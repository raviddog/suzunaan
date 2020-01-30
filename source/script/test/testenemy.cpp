#include "testenemy.hpp"
#include <cmath>

namespace Game::Script::Test {
    void testenemyfunc(Game::enemy_s *enemy) {
        if(enemy->frames == 30) {
            enemy->accel = -0.2f;
        } else if(enemy->frames == 40) {
            enemy->accel = 0.f;
        }

        if(enemy->frames > 70) {
            if(enemy->frames % 3 == 0) {
                for(int j = 0; j < 12; j++) {
                    bullet_s *bullet = Game::enemy_s::getBullet();
                    if(bullet) {
                        bullet->type = 48 + j % 16;
                        bullet->active = true;
                        bullet->accel = 0.f;
                        bullet->x_pos = enemy->x_pos;
                        bullet->y_pos = enemy->y_pos;
                        bullet->angle = 360.f * sin((double)enemy->frames * 3.14159265/180) + j * 45.f;
                        bullet->speed = 4.f;
                        bullet->run_instructions = Game::Script::Test::testbulletfunc;
                    }
                }
            }
        }
    }
}
