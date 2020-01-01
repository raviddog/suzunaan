#ifndef _ENEMY_H
#define _ENEMY_H

#include <unordered_map>
#include "script.h"
#include "bullet.h"

namespace game::content {

    extern bullet_s* (*getBullet)();

    class enemy_s {
        private:

        public:
            bool active;
            int type, frames, hp;
            float x_pos, y_pos;

            float speed, angle;
            float accel, angle_change;
            float draw_angle;


            int current_instr = -1;
            std::unordered_map<int, script_instruction*> *instructions;

            void update();
            
            //  no need for a reset, don't need to recycle enemy objects
            enemy_s();


    };


}



#endif