#include "bullet.h"
#include "math.h"
#include <string>

#define PI 3.14159265

namespace game::content {
    float bullet_bounds_x = 0.f, bullet_bounds_y = 0.f, bullet_bounds_xmax = 640.f, bullet_bounds_ymax = 480.f;
    
    void bullet_s::update() {
        if(active) {
            frames++;

            //  apply instructions
            if(instructions != nullptr) {
                //  check if instruction exists
                if(instructions->count(frames) > 0) {
                    std::string instr = instructions->at(frames);
                    
                    //  parse string
                    size_t start = 0, offset = 0;

                    while(start + offset < instr.length() && instr[start + offset] != '(') {
                        offset++;
                    }
                    
                    //  check instruction
                    if(instr.substr(start, offset) == "accel") {
                        offset++;
                        float val = std::strtof(instr.substr(start + offset).c_str(), nullptr);
                        accel = val;
                    } else if(instr.substr(start, offset) == "angle_change") {
                        offset++;
                        float val = std::strtof(instr.substr(start + offset).c_str(), nullptr);
                        angle_change = val;
                    } else if(instr.substr(start, offset) == "type_change") {
                        offset++;
                        int val = (int)std::strtol(instr.substr(start + offset).c_str(), nullptr, 0);
                        type += val;
                    }
                }

                //  perform constant instructions
                if(instructions->count(0) > 0) {
                    std::string instr = instructions->at(0);
                    //  parse string
                }
            }


            speed += accel;
            angle += angle_change;
            if(angle > 360.f) angle -= 360.f;
            x_pos += std::sin(angle * PI / 180.f) * speed;
            y_pos += std::cos(angle * PI / 180.f) * speed;

            //  update visual angle
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
}