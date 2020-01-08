#include "bullet.hpp"
#include "math.h"
#include "script.hpp"

#define PI 3.14159265

namespace game::content {
    //  #FIX 1
    float   bullet_bounds_x = 0.f,
            bullet_bounds_y = 0.f,
            bullet_bounds_xmax = 640.f,
            bullet_bounds_ymax = 480.f;

    bullet_s::bullet_s() {
        active_instructions = new std::vector<uint32_t>();
    }

    void bullet_s::reset() {
        active = false;
        type = 0;
        owner = -1;
        frames = 0;
        x_pos = 0.f;
        y_pos = 0.f;
        speed = 0.f;
        angle = 0.f;
        accel = 0.f;
        angle_change = 0.f;
        draw_angle = 0.f;
        instructions = nullptr;
        active_instructions->clear();
    }
    
    void bullet_s::update() {
        if(active) {
            //  apply instructions
            if(instructions != nullptr) {
                //  check frame triggers
                if(instructions->frame_triggers->count(frames) > 0) {
                    //  add instructions from the vector to the active instructions
                    std::vector<uint32_t> *ins = instructions->frame_triggers->at(frames);
                    for(int i = 0; i < ins->size(); i++) {
                        active_instructions->push_back(ins->at(i));
                    }
                }
            }

            run_instructions();


            speed += accel;
            angle += angle_change;
            if(angle > 360.f) angle -= 360.f;
            if(angle < -360.f) angle += 360.f;
            x_pos += std::sin(angle * PI / 180.f) * speed;
            y_pos += std::cos(angle * PI / 180.f) * speed;

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
        for(int i = 0; i < active_instructions->size();) {
            uint32_t num = active_instructions->at(i);
            //  get active script instruction from instruction list
            if(instructions->instructions->count(num) > 0) {
                //  instruction does exist
                script_instruction *current = instructions->instructions->at(num);
                //  in case the instruction deletes itself with stop()
                bool deleted = false;
                //  run each instruction
                for(int j = 0; j < current->instruct->size(); j++) {
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
                            int result = instr_stop(args.type_3);
                            if(result > -1) {
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
        angle_change = val;
    }

    void bullet_s::instr_type_set_relative(int val) {
        type += val;
    }

    void bullet_s::instr_angle(float val) {
        angle = val;
    }

    int bullet_s::instr_stop(uint32_t val) {
        int i = 0;
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
            return -1;
        }
    }

    void bullet_s::instr_start(uint32_t val) {
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


}