#include "player.hpp"

namespace Game {
    void player_s::init(float x, float y, float right, float top) {
        x_pos = (right - x) / 2.f + x;
        y_pos = (top - y) / 4.f + 3.f * y;
        animFrame = 0;
        animDelay = 0;

        x_max = right;
        y_max = top;
        x_min = x;
        y_min = y;

        speed_unf_straight = 4.f;
        speed_unf_diag = 2.82843f;
        speed_foc_straight = 2.f;
        speed_foc_diag = 1.41421f;

        focused = false;
        focused_rotation = 0.f;

        hitbox_radius = 0.5f;

        shoot_enabled = true;
        for(int i = 0; i < player_bullet_max; i++) {
            bullet[i].active_frame = 0;
            bullet[i].draw_frame = 0;
        }
    }

    void player_s::update(unsigned char keyState) {
        //  c shift x z right left down up
        //  0000 0000

        bool keys[8] = {
            keyState & 0x01,
            keyState & 0x02,
            keyState & 0x04,
            keyState & 0x08,
            keyState & 0x10,
            keyState & 0x20,
            keyState & 0x40,
            keyState & 0x80
        };

        enum key {
            up = 0,
            down,
            left,
            right,
            z,
            x,
            shift,
            c
        };

        //  time delay for animation update
        animDelay += 1;
        if(animDelay >= 4) {
            animDelay = 0;
            animFrame += 1;
        }

        //  movement direction animation changes
        if(keys[left] && moveDir != 1) {
            //  left pushed but not right, not moving left
            animDelay = 0;
            animFrame = 8;
            moveDir = 1;

        } else if(keys[right] && !keys[left] && moveDir != 2) {
            //  right pushed but not left, not moving right
            animDelay = 0;
            animFrame = 16;
            moveDir = 2;
        } else if(!keys[left] && !keys[right] && moveDir != 0) {
            //  neither right or left pushed, moving sideways
            animDelay = 0;
            animFrame = 0;
            moveDir = 0;
        }

        //  accelerate initial direction animation frames
        if((moveDir == 1 && animFrame < 12) || (moveDir == 2 && animFrame < 20)) {
            animFrame += 1;
            animDelay = 0;
        }

        //  roll over animation frames
        switch(moveDir) {
            case 0:
                if(animFrame >= 8) {
                    animFrame = 0;
                }
                break;
            case 1:
                if(animFrame >= 16) {
                    animFrame = 12;
                }
                break;
            case 2:
                if(animFrame >= 24) {
                    animFrame = 20;
                }
                break;
        }

        //  check focused
        float str, diag;
        if(keys[shift]) {
            str = speed_foc_straight;
            diag = speed_foc_diag;
            focused = true;
        } else {
            str = speed_unf_straight;
            diag = speed_unf_diag;
            focused = false;
        }

        if(focused) {
            focused_rotation += 2.f;
            if(focused_rotation > 360.f) focused_rotation -= 360.f;
        }

        //  apply movement
        if(keys[up] && !keys[right] && !keys[down] && !keys[left]) {
            //  up
            y_pos -= str;

        } else if(keys[up] && keys[right] && !keys[down] && !keys[left]) {
            //  up right
            y_pos -= diag;
            x_pos += diag;

        } else if(!keys[up] && keys[right] && !keys[down] && !keys[left]) {
            //  right
            x_pos += str;
            
        } else if(keys[right] && keys[down] && !keys[left]) {
            //  right down
            x_pos += diag;
            y_pos += diag;
            
        } else if(!keys[right] && keys[down] && !keys[left]) {
            //  down
            y_pos += str;
            
        } else if(keys[down] && keys[left]) {
            //  down left
            y_pos += diag;
            x_pos -= diag;
            
        } else if(!keys[up] && !keys[down] && keys[left]) {
            //  left
            x_pos -= str;
            
        } else if(keys[up] && !keys[down] && keys[left]) {
            //  left up
            x_pos -= diag;
            y_pos -= diag;
        }

        //  clamp position
        if(x_pos > x_max) x_pos = x_max;
        if(x_pos < x_min) x_pos = x_min;
        if(y_pos > y_max) y_pos = y_max;
        if(y_pos < y_min) y_pos = y_min;

        
        //  shooting logic
        //  move bullets, advance active frame, remove if out of bounds
        for(int i = 0; i < player_bullet_max; i++) {
            if(bullet[i].active_frame > 1) {
                bullet[i].active_frame += 1;
                bullet[i].y_pos -= 4.f;
                if(bullet[i].active_frame % 3 == 0) {
                    bullet[i].draw_frame += 1;
                    if(bullet[i].draw_frame > 4) {
                        bullet[i].active_frame = 0;
                    }
                }
            } else if(bullet[i].active_frame > 0) {
                bullet[i].y_pos -= 24.f;

                //  kill bullet if off the screen
                if(bullet[i].y_pos < y_min - 32.f) {
                    bullet[i].active_frame += 1;
                }
            }
        }

        //  fire new bullets
        if(!shoot_enabled) {
            shoot_enabled = true;
        } else {
            if(keys[z]) {
                shoot_enabled = false;
                
                //  spawn left bullet
                int bullet_num;
                for(bullet_num = 0; bullet_num < player_bullet_max && bullet[bullet_num].active_frame != 0; bullet_num++);
                if(bullet_num < player_bullet_max) {
                    bullet[bullet_num].x_pos = x_pos - 8.f;
                    bullet[bullet_num].y_pos = y_pos - 24.f;
                    bullet[bullet_num].active_frame = 1;
                    bullet[bullet_num].draw_frame = 0;
                    bullet[bullet_num].dmg = 4;
                }

                //  spawn right bullet
                for(bullet_num = 0; bullet_num < player_bullet_max && bullet[bullet_num].active_frame != 0; bullet_num++);
                if(bullet_num < player_bullet_max) {
                    bullet[bullet_num].x_pos = x_pos + 8.f;
                    bullet[bullet_num].y_pos = y_pos - 24.f;
                    bullet[bullet_num].active_frame = 1;
                    bullet[bullet_num].draw_frame = 0;
                    bullet[bullet_num].dmg = 4;
                }
            }
        }
    }
}