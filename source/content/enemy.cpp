#include "enemy.h"
#include "math.h"

#define PI 3.14159265

namespace game::content {
    bullet_s* (*getBullet)();

    float   enem_bounds_x = -20.f,
            enem_bounds_y = -20.f,
            enem_bounds_xmax = 660.f,
            enem_bounds_ymax = 500.f;

    void enemy_s::update() {
        if(active) {
            frames++;
            
            if(hp > 0) {
                if(instructions != nullptr) {
                    //  update current instruction
                    if(instructions->count(frames) > 0) {
                        current_instr = frames;
                    }
                }

                //  run current instruction
                if(current_instr != -1) {
                    for(int i = 0; i < instructions->at(current_instr)->instruct->size(); i++) {
                        
                    }
                }
            }
        }

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
    }

    enemy_s::enemy_s() {
        
    }
}