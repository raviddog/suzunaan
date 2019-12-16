#include "bullet.h"
#include "math.h"

#define PI 3.14159265

namespace game::content {
    
    void bullet_s::update() {
        if(active) {
            //  apply instructions
            if(instructions != nullptr) {
                if(instructions->count(frames) > 0) {
                    std::string instr = instructions->at(frames);
                    //  parse string
                }
            }

            frames++;

            speed += accel;
            angle += angle_change;
            x_pos += std::sin(angle * PI / 180.f) * speed;
            y_pos += std::cos(angle * PI / 180.f) * speed;

            //  update visual angle
            if(type == 7) {
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