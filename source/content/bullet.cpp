#include "bullet.h"
#include "math.h"
#include "script.h"

#define PI 3.14159265

namespace game::content {
    float   bullet_bounds_x = 0.f,
            bullet_bounds_y = 0.f,
            bullet_bounds_xmax = 640.f,
            bullet_bounds_ymax = 480.f;

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
    }
    
    void bullet_s::update() {
        if(active) {
            frames++;

            //  apply instructions
            if(instructions != nullptr) {
                //  check if instruction exists
                if(instructions->count(frames) > 0) {
                    script_instruction *instr = instructions->at(frames);
                    for(int i = 0; i < instr->instruct->size(); i++) {
                        run_instruction(instr, i);
                    }

                }

                //  perform constant instructions
                if(instructions->count(0) > 0) {
                    script_instruction *instr = instructions->at(0);
                    for(int i = 0; i < instr->instruct->size(); i++) {
                        run_instruction(instr, i);
                    }
                }
            }


            speed += accel;
            angle += angle_change;
            if(angle > 360.f) angle -= 360.f;
            x_pos += std::sin(angle * PI / 180.f) * speed;
            y_pos += std::cos(angle * PI / 180.f) * speed;

            //  update visual angle (spinning stars, etc)
            if(type > 159 && type < 176) {
                draw_angle += 6.f;
                if(draw_angle > 360.f) draw_angle -= 360.f;
            } else {
                draw_angle = angle;
            }


            //  self disable, hardcoded for now
            if(x_pos > bullet_bounds_xmax) active = false;
            if(x_pos < bullet_bounds_x) active = false;
            if(y_pos > bullet_bounds_ymax) active = false;
            if(y_pos < bullet_bounds_y) active = false;
        }
    }

    void bullet_s::run_instruction(script_instruction *instr, int i) {
        int type = instr->instruct->at(i);
        script_args args = instr->val->at(i);
        switch(type) {
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
            default:
                break;
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


    
}