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
            x_pos += std::sin(angle * PI / 180.f) * speed;
            y_pos += std::cos(angle * PI / 180.f) * speed;


            // x_speed += x_accel;
            // y_speed += y_accel;

            // x_pos += x_speed;
            // y_pos += y_speed;

            //  self disable, hardcoded for now
            if(x_pos > 640.f) active = false;
            if(x_pos < 0.f) active = false;
            if(y_pos > 480.f) active = false;
            if(y_pos < 0.f) active = false;
        }
    }
}