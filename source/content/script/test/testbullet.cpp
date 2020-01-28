#include "testbullet.hpp"

namespace game::script::test {
    void testbulletfunc(game::content::bullet_s *bullet) {
        if(bullet->frames == 30u) {
            bullet->accel = -0.25f;
        }

        if(bullet->frames == 41u) {
            bullet->accel = 0.f;
        }

        if(bullet->frames == 90u) {
            bullet->accel = 0.1f;
            bullet->angle_change = 5.f;
        }

        if(bullet->frames == 107u) {
            bullet->accel = 0.f;
        }

        if(bullet->frames == 160u) {
            bullet->angle_change = 0.f;
            bullet->speed = 0.5f;
            bullet->type += 112;
        }
    }
}