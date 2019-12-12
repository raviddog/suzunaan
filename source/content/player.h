#ifndef _PLAYER_H
#define _PLAYER_H

namespace game::content {
    struct bullet_s {
        //  base shot for now
        //  12x16
        //  8 pixels per frame, shoot every other frame
        int active_frame;
    };

    struct player_s {
        float x_pos, y_pos, x_max, y_max;
        float speed_unf_straight, speed_unf_diag, speed_foc_straight, speed_foc_diag;
        int animFrame, animDelay, moveDir;
        //  moveDir
        //  1 0 2

        bullet_s bullet[42];
        
        void init(float, float);
        void update(unsigned char);
    };

}


#endif