#include "bullet.hpp"
#include <cmath>
#include "player.hpp"

namespace Game {
    //  #FIX 1
    float   bullet_bounds_x = 0.f,
            bullet_bounds_y = 0.f,
            bullet_bounds_xmax = 640.f,
            bullet_bounds_ymax = 480.f;
    
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

    void bullet_s::reset() {
        active = false;
        type = 0;
        owner = -1;
        frames = 0;
        x_pos = 0.f;
        y_pos = 0.f;
        speed = 0.f;
        angle = 0.f;
        angle_change = 0.f;
        accel = 0.f;
        draw_angle = 0.f;
        run_instructions = nullptr;
        storage[0].u = 0;
        storage[1].u = 0;
        storage[2].u = 0;
        storage[3].u = 0;
    }
    
    void bullet_s::update() {
        if(active) {
            if(run_instructions) run_instructions(this);

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