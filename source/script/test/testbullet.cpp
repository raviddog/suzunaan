#include "testbullet.hpp"

#include <cmath>

namespace Game::Script::Test {
    void testbulletfunc(Game::bullet_s *bullet) {

        if(bullet->y_pos < 0.f) {
            bullet->angle = 180.f - bullet->angle;
        }

        if(bullet->x_pos < 0.f || bullet->x_pos > 384.f) {
            bullet->angle = 360.f - bullet->angle;
        }

        if(bullet->angle > 180.f) {
            bullet->angle += 0.1f;
        } else {
            bullet->angle -= 0.1f;
        }
    }
}