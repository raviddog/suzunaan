#include "testbullet.hpp"
#include <cmath>

namespace Game::Script::Test {
    void testbulletfunc(Game::bullet_s *bullet) {
        // float anglePlayer = bullet->instr_getAngleToPlayer();
        // if(std::abs(bullet->angle - anglePlayer) < 1.f) {
        //     bullet->angle = anglePlayer;
        // } else {
        //     if(bullet->angle - anglePlayer > 0.f) {
        //         bullet->angle += 1.f;
        //     } else {
        //         bullet->angle -= 1.f;
        //     }
        // }

        // if(bullet->frames == 0u) {
        //     bullet->storage[0].b = false;
        // }

        if(bullet->y_pos < 8.f || bullet->y_pos > 472.f) {
            bullet->angle = 180.f - bullet->angle;
        }

        if(bullet->x_pos < 8.f || bullet->x_pos > 632.f) {
            bullet->angle = 360.f - bullet->angle;
        }

        // if(!bullet->storage[0].b) {
        //     if(bullet->y_pos < 8.f || bullet->y_pos > 432.f || bullet->x_pos < 8.f || bullet->x_pos > 632.f) {
        //         bullet->angle = bullet->instr_getAngleToPlayer() + 5.f;
        //         bullet->speed = 2.f;
        //         // bullet->storage[0].b = true;
        //     }
        // }

        // if(bullet->frames == 30u) {
        //     bullet->accel = -0.25f;
        // }

        // if(bullet->frames == 41u) {
        //     bullet->accel = 0.f;
        // }

        // if(bullet->frames == 90u) {
        //     bullet->accel = 0.1f;
        //     bullet->angle_change = 5.f;
        // }

        // if(bullet->frames == 107u) {
        //     bullet->accel = 0.f;
        // }

        // if(bullet->frames == 160u) {
        //     bullet->angle_change = 0.f;
        //     bullet->speed = 0.5f;
        //     bullet->type += 112;
        // }
    }
}