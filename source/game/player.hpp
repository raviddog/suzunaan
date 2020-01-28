#ifndef _PLAYER_H
#define _PLAYER_H

namespace Game {

    //  bullets part of player class for now
    //  move it to actual bullet mechanism later
    struct bullet_player_s {
        //  base shot for now
        //  12x16
        //  8 pixels per frame, shoot every other frame
        //
        
        //  0   disabled
        //  1   active
        //  2-4  fading out phase
        int active_frame, draw_frame;
        float x_pos, y_pos;

    };

    struct player_s {
        float x_pos, y_pos, x_max, y_max, x_min, y_min;
        float speed_unf_straight, speed_unf_diag, speed_foc_straight, speed_foc_diag;
        int animFrame, animDelay, moveDir;
        bool focused;
        float focused_rotation, hitbox_radius;
        //  moveDir
        //  1 0 2

        bullet_player_s bullet[42];
        bool shoot_enabled;
        
        void init(float, float, float, float);
        void update(unsigned char);
    };

}


#endif