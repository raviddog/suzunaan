#ifndef _ENEMY_H
#define _ENEMY_H

#include <unordered_map>
// #include <cstdlib>
#include "bullet.h"

namespace game::content {

    extern bullet_s* (*getBullet)();

    class enemy {
        private:

        public:
            bool active;
            int type, frames, hp;
            float x_pos, y_pos;
            std::string current_instr;
            std::unordered_map<int, std::string> *instructions;

            void update();


    };


}



#endif