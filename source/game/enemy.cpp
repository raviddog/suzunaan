#include "enemy.hpp"
#include <cmath>
#include <vector>

#define PI 3.14159265

namespace Game{
    // bullet_s* (*enemy_s::getBullet)();
    // std::vector<enemy_s*> *enemy_s::enemy_draw = nullptr;

    //  #FIX 1
    float   enem_bounds_x = -20.f,
            enem_bounds_y = -20.f,
            enem_bounds_xmax = 660.f,
            enem_bounds_ymax = 500.f;
    

    //  #FIX 51
    //  create a common functions engine module and put these in it later
    template <class T>
    T between(T min, T max);
    float toRadians(float deg);
    float toDegrees(float rad);

    enemy_s::enemy_s() {
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
        run_instructions = nullptr;
    }

    void enemy_s::activate() {
        active = true;
        enemy_draw->push_back(this);
    }

    void enemy_s::update() {
        if(active) {
            if(run_instructions) run_instructions(this);

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
}