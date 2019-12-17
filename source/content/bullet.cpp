#include "bullet.h"
#include "math.h"
#include <string>

#define PI 3.14159265

namespace game::content {
    
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
            if(x_pos > 640.f) active = false;
            if(x_pos < 0.f) active = false;
            if(y_pos > 480.f) active = false;
            if(y_pos < 0.f) active = false;
        }
    }
}