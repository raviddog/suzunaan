#include "enemy.h"

namespace game::content {
    bullet_s* (*getBullet)();

    void enemy::update() {
        if(active) {
            if(hp > 0) {
                if(instructions != nullptr) {
                    if(instructions->count(frames) > 0) {
                        current_instr = instructions->at(frames);
                    }
                }

                //  parse instruction
            }
        }
    }
}