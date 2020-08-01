#include "enemy.hpp"

#include <cmath>
#include <vector>

#define PI 3.14159265


namespace Game{
    // bullet_s* (*enemy_s::getBullet)();
    extern std::unordered_map<uint32_t, std::shared_ptr<bullet_script>> *bullet_scripts;
    // std::vector<enemy_s*> *enemy_s::enemy_draw = nullptr;

    //  #FIX 1
    float   enem_bounds_x = -8.f,
            enem_bounds_y = 0.f,
            enem_bounds_xmax = 392.f,
            enem_bounds_ymax = 448.f;
    

    //  #FIX 51
    //  create a common functions engine module and put these in it later
    template <class T>
    T between(T min, T max);
    float toRadians(float deg);
    float toDegrees(float rad);

    enemy_s::enemy_s() {
        active_instructions = new std::vector<uint32_t>();
        cust_triggers = new std::unordered_multimap<uint32_t, uint32_t>();
        cancel_cust_triggers = new std::unordered_set<uint32_t>();
        listener_triggers = nullptr;

        active = false;
        id = -1;
        type = 0u;
        frames = 0u;
        hp = 0;
        x_pos = 320.f;
        y_pos = 0.f;
        speed = 0.f;
        angle = 0.f;
        accel = 0.f;
        angle_change = 0.f;
        draw_angle = 0.f;
    }

    enemy_s::~enemy_s() {
        delete active_instructions;
        delete cust_triggers;
        delete cancel_cust_triggers;
        delete listener_triggers;
    }

    void enemy_s::activate() {
        if(instructions) {
            //  run init trigger
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
        run_instructions();
        active = true;
        enemy_draw->push_back(this);
    }

    void enemy_s::update() {
        if(active) {            
            if(hp > 0) {
                if(instructions) {
                    if(instructions->frame_triggers->count(frames) > 0) {
                        //  add instructions from the vector to the active instructions
                        std::vector<uint32_t> *ins = instructions->frame_triggers->at(frames);
                        for(size_t i = 0; i < ins->size(); i++) {
                            active_instructions->push_back(ins->at(i));
                        }
                    }
                    //  check listener triggers
                    if(listener_triggers) {
                        //  dunno yet
                    }
                }

                //  run custom frame triggers
                if(cust_triggers->count(frames) > 0) {
                    //  could've used auto lol
                    std::pair<  std::unordered_multimap<uint32_t, uint32_t>::iterator,
                                std::unordered_multimap<uint32_t, uint32_t>::iterator> range = cust_triggers->equal_range(frames);
                    for(auto it = range.first; it != range.second; ++it) {
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

                
            } else {
                active = false;
            }

            run_instructions();

            speed += accel;
            angle += angle_change;
            if(angle > 360.f) angle -= 360.f;
            x_pos += std::sin(angle * PI / 180.f) * speed;
            y_pos += std::cos(angle * PI / 180.f) * speed;

            if(hp <= 0) active = false;


            //  self disable, hardcoded for now
            //  not sure how to actually do this, maybe ill just make the enemy take damage when its outside the screen
            //  because i might find myself storing enemies offscreen for a while and stuff
            //  before they enter the screen later
            if(x_pos > enem_bounds_xmax) active = false;
            if(x_pos < enem_bounds_x) active = false;
            if(y_pos > enem_bounds_ymax) active = false;
            if(y_pos < enem_bounds_y) active = false;

            ++frames;
        }
    }

     void enemy_s::run_instructions() {
        for(size_t i = 0; i < active_instructions->size();) {
            uint32_t num = active_instructions->at(i);
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
                            instr_angle(args.type_1);
                            break;
                        case 3:
                            instr_accel(args.type_1);
                            break;
                        case 4:
                            instr_bullet(args.type_3);
                            break;
                        case 5:
                            instr_setHP(args.type_2);
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
                            std::pair<uint32_t, uint32_t> val = script_getIntInt(args.type_4);
                            instr_frameTrigger(val.first, val.second);
                            break;
                        }
                        case 9:
                        {
                            std::pair<uint32_t, uint32_t> val = script_getIntInt(args.type_4);
                            instr_frameTriggerOffset(val.first, val.second);
                            break;
                        }
                        case 10:
                            instr_stopInterval(args.type_3);
                            break;
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

    void enemy_s::instr_speed(float val) {
        speed = val;
    }

    void enemy_s::instr_angle(float val) {
        angle = val;
    }

    void enemy_s::instr_accel(float val) {
        accel = val;
    }

    void enemy_s::instr_bullet(uint32_t spawnID) {
        if(instructions->bullet_spawns->count(spawnID) > 0) {
            bullet_spawn bs = instructions->bullet_spawns->at(spawnID);
            bullet_s *bullet = getBullet();
            if(bullet) {
                //  initialise bullet with spawn values
                bullet->type = bs.type;
                bullet->x_pos = x_pos + bs.x_offset;
                bullet->y_pos = y_pos + bs.y_offset;
                bullet->speed = bs.speed;
                bullet->angle = bs.angle;
                bullet->accel = bs.accel;
                bullet->angle_change = bs.angle_change;
                bullet->instructions = nullptr;

                if(bs.scriptID > 0) {
                    if(bullet_scripts->count(bs.scriptID) > 0) {
                        //  temp, replace with proper smart pointers later
                        bullet->instructions = (bullet_scripts->at(bs.scriptID)).get();
                    }
                }
            }
        }
    }

    void enemy_s::instr_setHP(int newhp) {
        hp = newhp;
    }

    size_t enemy_s::instr_stop(uint32_t val) {
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

    void enemy_s::instr_start(uint32_t val) {
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

    void enemy_s::instr_frameTrigger(uint32_t id, uint32_t frame) {
        if(instructions->id_map->count(id) > 0) {
            id = instructions->id_map->at(id);
        }
        cust_triggers->insert({frame, id});
    }

    void enemy_s::instr_frameTriggerOffset(uint32_t id, uint32_t frame) {
        if(instructions->id_map->count(id) > 0) {
            id = instructions->id_map->at(id);
        }
        cust_triggers->insert({frame + frames, id});
    }

    void enemy_s::instr_stopInterval(uint32_t id) {
        //  can only have 1 of each element in a set, so no need to check if it already exists
        cancel_cust_triggers->insert(id);
    }
}