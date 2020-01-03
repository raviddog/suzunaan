#ifndef _ENEMY_H
#define _ENEMY_H

#include <unordered_map>
#include "script.h"
#include "bullet.h"

namespace game::content {

    extern bullet_s* (*getBullet)();

    class enemy_s {
        private:
            void run_instruction(script_instruction*, int);

            //  script instructions
            void instr_speed(float);

        public:
            bool active;
            int type, frames, hp;
            float x_pos, y_pos;

            float speed, angle;
            float accel, angle_change;
            float draw_angle;


            script_instruction* current_instr;
            std::unordered_map<int, script_instruction*> *instructions;

            void update();
            
            //  no need for a reset, don't need to recycle enemy objects
            //  use a constructor instead to set default values
            //  dont need a destructor
            enemy_s();


    };


}



#endif