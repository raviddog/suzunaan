#include "testenemy.hpp"

#include "testbullet.hpp"

#include <cmath>

template <class T> T between(T min, T max) {
    return (T)(((float)max - (float)min) * (float)rand() / (float)RAND_MAX + (float)min);
}

namespace Game::Script::Test {
    void testenemyfunc(Game::enemy_s *enemy) {
        if(enemy->frames == 0) {
            enemy->storage[0].b = false;
            enemy->storage[1].b = false;
            enemy->storage[2].i = 0;
        }
        if(enemy->frames == 30) {
            enemy->accel = -0.2f;
        } else if(enemy->frames == 40) {
            enemy->accel = 0.f;
        }

        if(enemy->frames > 70) {
            if(enemy->frames % 30 == 0) {
                enemy->storage[0].b = true;
                enemy->storage[1].b = true;
                enemy->storage[2].i = 0;
                // for(int j = 0; j < 12; j++) {
                //     bullet_s *bullet = Game::enemy_s::getBullet();
                //     if(bullet) {
                //         bullet->type = 48 + j % 16;
                //         bullet->active = true;
                //         bullet->accel = 0.f;
                //         bullet->x_pos = enemy->x_pos;
                //         bullet->y_pos = enemy->y_pos;
                //         bullet->angle = 360.f * sin((double)enemy->frames * 3.14159265/180) + j * 45.f;
                //         bullet->speed = 4.f;
                //         bullet->run_instructions = Game::Script::Test::testbulletfunc;
                //     }
                // }
            }
            if(enemy->storage[0].b) {
                bullet_s *b = Game::enemy_s::getBullet();
                if(b) {
                    b->type = BTIcicle + enemy->storage[2].i;
                    b->active = true;
                    b->y_pos = 120.f;
                    b->speed = 1.f;
                    b->accel = 0.02f;
                    b->run_instructions = testbulletfunc;
                    if(enemy->storage[1].b) {
                        b->angle = between<float>(0.f, 40.f);
                        b->x_pos = enemy->storage[2].i * 40.f + between<float>(10.f, 30.f);
                        ++enemy->storage[2].i;
                    } else {
                        b->angle = between<float>(-40.f, 0.f);
                        b->x_pos = enemy->storage[2].i * 40.f + between<float>(10.f, 30.f);
                        --enemy->storage[2].i;
                    }
                }

                if(enemy->storage[2].i == 15) {
                    enemy->storage[1].b = false;
                }
                if(!enemy->storage[1].b && enemy->storage[2].i == 0) {
                    enemy->storage[0].b = false;
                }
            }
        }
    }
}
