#include "bullet.hpp"

#include "player.hpp"

#include <cmath>

namespace Game {
    //  #FIX 1
    float   bullet_bounds_x = -8.f,
            bullet_bounds_y = 0.f,
            bullet_bounds_xmax = 392.f,
            bullet_bounds_ymax = 448.f;
    
    template <class T>
    T between(T min, T max) {
        return (max - min) * (float)rand() / (float)RAND_MAX + min;
    }

    float toRadians(float deg) {
        return deg * 3.14159265f / 180.f;
    }

    float toDegrees(float rad) {
        return rad * 180.f / 3.14159265f;
    }

    bullet_s::bullet_s() {
        active_instructions = new std::vector<int>();
        cust_triggers = new std::multimap<int, int>();
        cancel_cust_triggers = new std::set<int>();
        listener_triggers = nullptr;
        reset();
    }

    bullet_s::~bullet_s() {
        if(active_instructions) delete active_instructions;
        if(cust_triggers) delete cust_triggers;
        if(cancel_cust_triggers) delete cancel_cust_triggers;
        if(listener_triggers) delete listener_triggers;
        
        active_instructions = nullptr;
        cust_triggers = nullptr;
        cancel_cust_triggers = nullptr;
        listener_triggers = nullptr;
    }
    void bullet_s::reset() {
        active = false;
        type = 0;
        owner = -1;
        frames = -15;
        x_pos = 0.f;
        y_pos = 0.f;
        speed = 0.f;
        angle = 0.f;
        angle_change = 0.f;
        accel = 0.f;
        draw_angle = 0.f;

        //  script stuff
        instructions = nullptr;
        active_instructions->clear();
        cust_triggers->clear();
        cancel_cust_triggers->clear();
        if(listener_triggers != nullptr) {
            delete listener_triggers;
            listener_triggers = nullptr;
        }
        //  compiled stuff
        // run_instructions = nullptr;
        storage[0].u = 0;
        storage[1].u = 0;
        storage[2].u = 0;
        storage[3].u = 0;
    }

    void bullet_s::setPos(float x_pos, float y_pos) {
        this->x_pos = x_pos;
        this->y_pos = y_pos;
    }

    void bullet_s::setPos(float x_pos, float y_pos, float speed, float angle) {
        this->x_pos = x_pos;
        this->y_pos = y_pos;
        this->speed = speed;
        this->angle = angle;
    }

    void bullet_s::update() {
        if(active) {
            //  make a copy of the non-frame trigger listeners if none exists
            //  also run init trigger here
            if(instructions && !listener_triggers) {
                listener_triggers = new auto(*(instructions->listeners));
                if(listener_triggers->count(2u) > 0) {
                    auto range = listener_triggers->equal_range(2u);
                    auto it = range.first;
                    while(it != range.second) {
                        //  activate all
                        active_instructions->push_back(it->second.second);
                        it = listener_triggers->erase(it);
                    }
                }
            }

            //  apply instructions
            if(instructions) {
                //  check frame triggers
                if(instructions->frame_triggers->count(frames) > 0) {
                    //  add instructions from the vector to the active instructions
                    std::vector<int> *ins = instructions->frame_triggers->at(frames);
                    for(size_t i = 0; i < ins->size(); i++) {
                        active_instructions->push_back(ins->at(i));
                    }
                }
                //  check listener triggers
                if(frames >= 0) {
                    if(listener_triggers) {
                        //  probably just do listeners manually here
                        //  there won't be enough of them to justify making separate functions?
                        
                        //  get distance
                        //  might wanna do this distance check when all the player collision tests are done
                        float distanceSQ = (y_pos - player->y_pos) * (y_pos - player->y_pos)
                                            + (x_pos - player->x_pos) * (x_pos - player->x_pos);
                        //  distanceToPlayer
                        auto range = listener_triggers->equal_range(1u);
                        auto it = range.first;
                        while(it != range.second) {
                            //  get value
                            float val = it->second.first.type_1;
                            if(distanceSQ < val * val) {
                                //  activate and remove
                                active_instructions->push_back(it->second.second);
                                it = listener_triggers->erase(it);
                            } else {
                                it++;
                            }                     
                        }
                    }
                }
                //  run custom frame triggers
                if(cust_triggers->count(frames) > 0) {
                    //  could've used auto lol
                    std::pair<  std::multimap<int, int>::iterator,
                                std::multimap<int, int>::iterator> range = cust_triggers->equal_range(frames);
                    for(auto it = range.first; it != range.second; it++) {
                        //  check if this trigger is to be canceled
                        if(cancel_cust_triggers->count(it->second) > 0) {
                            //  don't run it
                            //  since it's a frame trigger, it can't be reached again, so there's no need to delete it
                            //  remove the cancel though, so that this trigger can be created again if so
                            cancel_cust_triggers->erase(it->second);
                        } else {
                            active_instructions->push_back(it->second);
                        }
                    }
                }
            }
                
            run_instructions();

            speed += accel;
            angle += angle_change;
            if(angle > 360.f) angle -= 360.f;
            if(angle < -360.f) angle += 360.f;
            x_pos += std::sin(toRadians(angle)) * speed;
            y_pos += std::cos(toRadians(angle)) * speed;

            //  update visual angle (spinning stars, etc)
            if(type > 159 && type < 176) {
                draw_angle += 6.f;
                if(draw_angle > 360.f) draw_angle -= 360.f;
            } else {
                draw_angle = angle;
            }


            //  disable when out of bounds
            if(x_pos > bullet_bounds_xmax) active = false;
            if(x_pos < bullet_bounds_x) active = false;
            if(y_pos > bullet_bounds_ymax) active = false;
            if(y_pos < bullet_bounds_y) active = false;

            
            frames++;
        }
    }

    void bullet_s::run_instructions() {
        for(size_t i = 0; i < active_instructions->size();) {
            int num = active_instructions->at(i);
            //  get active script instruction from instruction list
            if(instructions->instructions->count(num) > 0) {
                //  instruction does exist
                script_instruction *current = instructions->instructions->at(num);
                //  in case the instruction deletes itself with stop()
                bool deleted = false;
                //  run each instruction
                for(size_t j = 0; j < current->instruct->size(); j++) {
                    script_args args = current->val->at(j);
                    switch(current->instruct->at(j)) {
                        case 1:
                            instr_speed(args.type_1);
                            break;
                        case 2:
                            instr_accel(args.type_1);
                            break;
                        case 3:
                            instr_angle_change(args.type_1);
                            break;
                        case 4:
                            instr_type_set_relative(args.type_2);
                            break;
                        case 5:
                            instr_angle(args.type_1);
                            break;
                        case 6:
                        {
                            //  if this removes an element before the current instruction, need to decrement i
                            size_t result = instr_stop(args.type_3);
                            if(result != active_instructions->size()) {
                                if(result < i) {
                                    i--;
                                } else if(result == i) {
                                    //  removed the instruction we're currently dealing with
                                    //  the instruction is already gone, so we can skip the selfdestruct check
                                    //  we dont want to permanently modify the instruction though
                                    deleted = true;
                                }
                            }
                            break;   
                        }
                        case 7:
                            instr_start(args.type_3);
                            break;
                        case 8:
                        {
                            std::pair<int, int> val = script_getIntInt(args.type_4);
                            instr_frameTrigger(val.first, val.second);
                            break;
                        }
                        case 9:
                        {
                            std::pair<int, int> val = script_getIntInt(args.type_4);
                            instr_frameTriggerOffset(val.first, val.second);
                            break;
                        }
                        case 10:
                            instr_stopInterval(args.type_3);
                            break;
                        case 11:
                            instr_angle_atPlayer();
                            break;
                        case 12:
                        {
                            std::pair<float, float> val = script_getFloatFloat(args.type_5);
                            instr_random_angle_change(val.first, val.second);
                            break;
                        }
                        case 13:
                        {
                            std::pair<float, float> val = script_getFloatFloat(args.type_5);
                            instr_random_angle(val.first, val.second);
                            break;
                        }
                        default:
                            break;
                    }
                }
                //  iterate i if the instruction isn't self destructing, else remove
                if(!deleted) {
                    if(current->selfdestruct) {
                        auto loc = active_instructions->begin() + i;
                        active_instructions->erase(loc);
                    } else {
                        i++;
                    }
                }
            } else {
                //  instruction doesn't exist, remove it from active
                auto loc = active_instructions->begin() + i;
                active_instructions->erase(loc);
            }
        }
    }

    void bullet_s::instr_speed(float val) {
        speed = val;
    }

    void bullet_s::instr_accel(float val) {
        accel = val;
    }

    void bullet_s::instr_angle_change(float val) {
        angle += val;
    }

    void bullet_s::instr_type_set_relative(int val) {
        type += val;
    }

    void bullet_s::instr_angle(float val) {
        angle = val;
    }

    size_t bullet_s::instr_stop(int val) {
        size_t i = 0;
        for(;i < active_instructions->size(); i++) {
            //  found instruction
            if(active_instructions->at(i) == val) break;
        }
        if(i != active_instructions->size()) {
            //  instruction is active, remove it
            auto loc = active_instructions->begin() + i;
            active_instructions->erase(loc);
            return i;
        } else {
            //  return last element + 1
            return active_instructions->size();
        }
    }

    void bullet_s::instr_start(int val) {
        //  first check if instruction is a real instruction
        if(instructions->instructions->count(val) > 0) {
            //  don't duplicate instructions, search active array to check if instruction is already running
            auto i = active_instructions->begin();
            for(; i != active_instructions->end(); i++) {
                //  found instruction
                if(*i == val) break;
            }
            if(i == active_instructions->end()) {
                //  not found, activate
                active_instructions->push_back(val);
            }
        }
    }

    void bullet_s::instr_frameTrigger(int id, int frame) {
        cust_triggers->insert({frame, id});
    }

    void bullet_s::instr_frameTriggerOffset(int id, int frame) {
        cust_triggers->insert({frame + frames, id});
    }

    void bullet_s::instr_stopInterval(int id) {
        //  can only have 1 of each element in a set, so no need to check if it already exists
        cancel_cust_triggers->insert(id);
    }

    void bullet_s::instr_angle_atPlayer() {
        //  get angle to player
        //  the order is all messed up because my angles go clockwise and my Y-axis is flipped
        float distWidth = x_pos - player->x_pos;
        float distHeight = player->y_pos - y_pos;
        float angle_rad;

        if(distWidth != 0.f) {
            angle_rad = (float)atan2(distHeight, distWidth);
            angle = toDegrees(angle_rad) - 90.f;
        } else {
            //  vertically lined up
            if(y_pos < player->y_pos) {
                //  underneath
                angle = 0.f;
            } else {
                angle = 180.f;
            }

        }
    }

    void bullet_s::instr_random_angle_change(float min, float max) {
        angle += between<float>(min, max);
    }

    void bullet_s::instr_random_angle(float min, float max) {
        angle = between<float>(min, max);
    }

    //  compiled script update function
    /*
    void bullet_s::update() {
        if(active) {
            if(run_instructions) run_instructions(this);

            speed += accel;
            angle += angle_change;
            if(angle > 360.f) angle -= 360.f;
            if(angle < 0.f) angle += 360.f;
            x_pos += std::sin(toRadians(angle)) * speed;
            y_pos += std::cos(toRadians(angle)) * speed;

            //  update visual angle (spinning stars, etc)
            if(type >= BTStar && type <= (BTStar + BCGrey)) {
                draw_angle += 6.f;
                if(draw_angle > 360.f) draw_angle -= 360.f;
            } else {
                draw_angle = angle + 180.f;
            }

            //  disable when out of bounds
            if(x_pos > bullet_bounds_xmax) active = false;
            if(x_pos < bullet_bounds_x) active = false;
            if(y_pos > bullet_bounds_ymax) active = false;
            if(y_pos < bullet_bounds_y) active = false;
            
            ++frames;
        }
    }
    */

    float bullet_s::instr_getAngleToPlayer() {
        //  get angle to player
        //  the order is all messed up because my angles go clockwise and my Y-axis is flipped
        float distWidth = x_pos - player->x_pos;
        float distHeight = player->y_pos - y_pos;
        float angle_rad;

        if(distWidth != 0.f) {
            angle_rad = (float)atan2(distHeight, distWidth);
            return toDegrees(angle_rad) - 90.f;
        } else {
            //  vertically lined up
            if(y_pos < player->y_pos) {
                //  underneath
                return 0.f;
            } else {
                return 180.f;
            }

        }
    }
}