#ifndef _ENEMY_H
#define _ENEMY_H

#include "bullet.hpp"
#include <vector>
#include "stdint.h"

namespace Game {
    
    class enemy_s {
        public:
            enemy_s();
            void activate();
            void update();

            //  static functions
            static bullet_s* (*getBullet)();
            static std::vector<enemy_s*> *enemy_draw;
            //  do i need an array for existing enemies
            //  don't for now

            bool active;
            int id, type, frames, hp;
            float x_pos, y_pos;

            float speed, angle;
            float accel, angle_change;
            float draw_angle;
            
            //  compiled scripts shouldn't need instruction maps
            //  instead just provide some storage variables
            union storage_u {
                uint32_t u;
                int32_t i;
                float f;
                bool b;
            };

            //  adjust this to the number of storage variables needed
            storage_u storage[6];
            //  pointer to the correct instruction function
            void (*run_instructions)(enemy_s*);            
            //  helper functions that the instruction function can use
            // float instr_angleToPlayer();
    };


}



#endif