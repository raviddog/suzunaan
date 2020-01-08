#ifndef _BULLET_H
#define _BULLET_H

#include "script.hpp"

namespace game::content {
    extern float bullet_bounds_x, bullet_bounds_y, bullet_bounds_xmax, bullet_bounds_ymax;

    class bullet_s {
        private:
            void run_instructions();
            //  bullet instruction functions
            void instr_speed(float);
            void instr_accel(float);
            void instr_angle_change(float);
            void instr_type_set_relative(int);
            void instr_angle(float);
            //  needs to inform the instruction loop that an element was removed
            int instr_stop(uint32_t);  
            void instr_start(uint32_t);

        public:
            bullet_s();
            bool active;
            uint32_t type, frames;
            int owner;
            float x_pos, y_pos;
            
            //  not sure which way to implement movement
            //  should accel be per second or per frame
            //  probably per frame, but then the values have to be super small
            float speed, angle;
            float accel, angle_change;
            float draw_angle;

            bullet_script *instructions = nullptr;
            std::vector<uint32_t> *active_instructions = nullptr;

            void reset();
            void update();

    };
}


#endif