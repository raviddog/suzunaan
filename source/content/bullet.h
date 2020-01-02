#ifndef _BULLET_H
#define _BULLET_H

#include <unordered_map>
#include "script.h"

namespace game::content {
    extern float bullet_bounds_x, bullet_bounds_y, bullet_bounds_xmax, bullet_bounds_ymax;

    class bullet_s {
        private:
            void run_instruction(script_instruction*, int);
            //  bullet instruction functions
            void instr_speed(float);
            void instr_accel(float);
            void instr_angle_change(float);
            void instr_type_set_relative(int);
            void instr_angle(float);

        public:
            bool active;
            int type, frames, owner;
            float x_pos, y_pos;
            
            //  not sure which way to implement movement
            //  should accel be per second or per frame
            //  probably per frame, but then the values have to be super small
            float speed, angle;
            float accel, angle_change;
            float draw_angle;

            std::unordered_map<int, script_instruction*> *instructions;
            void reset();
            void update();

    };
}


#endif