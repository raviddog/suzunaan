#ifndef _BULLET_H
#define _BULLET_H

#include <unordered_map>
#include "script.h"

namespace game::content {
    extern float bullet_bounds_x, bullet_bounds_y, bullet_bounds_xmax, bullet_bounds_ymax;

    struct bullet_s {
        bool active;
        int type, frames, owner;
        float x_pos, y_pos;
        
        //  not sure which way to implement movement
        //  should accel be per second or per frame
        //  probably per frame, but then the values have to be super small
        float speed, angle;
        float accel, angle_change;
        float draw_angle;

        std::unordered_map<int, script_instruction_bullet*> *instructions;
        void reset();
        void update();

        //  bullet instruction functions
        void instr_accel(float);
        void instr_angle_change(float);
        void instr_type_change(float);
        void instr_speed(float);
    };
}


#endif