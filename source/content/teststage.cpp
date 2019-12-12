#include "engine/engine.h"

using namespace engine;

namespace game::teststage {

    //  move a bunch of these things to separate files later

    //  player
    SpriteSheet *img_player;

    struct player_bullet_s {
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
        
        void init(float, float);
        void update(unsigned char);
    } dat_player;

    void player_s::init(float width, float height) {
        x_pos = width / 2.f;
        y_pos = height / 4.f;
        animFrame = 0;
        animDelay = 0;

        x_max = width;
        y_max = height;

        speed_unf_straight = 4.f;
        speed_unf_diag = 2.82843f;
        speed_foc_straight = 2.f;
        speed_foc_diag = 1.41421f;
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
        if(keys[left] && !keys[right] && moveDir != 1) {
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
        } else {
            str = speed_unf_straight;
            diag = speed_unf_diag;
        }

        //  apply movement
        if(keys[up] && !keys[right] && !keys[down] && !keys[left]) {
            //  up
            y_pos += str;

        } else if(keys[up] && keys[right] && !keys[down] && !keys[left]) {
            //  up right
            y_pos += diag;
            x_pos += diag;

        } else if(!keys[up] && keys[right] && !keys[down] && !keys[left]) {
            //  right
            x_pos += str;
            
        } else if(!keys[up] && keys[right] && keys[down] && !keys[left]) {
            //  right down
            x_pos += diag;
            y_pos -= diag;
            
        } else if(!keys[up] && !keys[right] && keys[down] && !keys[left]) {
            //  down
            y_pos -= str;
            
        } else if(!keys[up] && !keys[right] && keys[down] && keys[left]) {
            //  down left
            y_pos -= diag;
            x_pos -= diag;
            
        } else if(!keys[up] && !keys[right] && !keys[down] && keys[left]) {
            //  left
            x_pos -= str;
            
        } else if(keys[up] && !keys[right] && !keys[down] && keys[left]) {
            //  left up
            x_pos -= diag;
            y_pos += diag;
        }

        //  clamp position
        if(x_pos > x_max) x_pos = x_max;
        if(x_pos < 0.f) x_pos = 0.f;
        if(y_pos > y_max) y_pos = y_max;
        if(y_pos < 0.f) y_pos = 0.f;

        

    }
    
    player_bullet_s bullet[42];

    void logic() {
        //  c shift x z right left down up
        unsigned char key = 0x00;
        if(engine::keyState[engine::kbUp]) key = key | 0x01;
        if(engine::keyState[engine::kbDown]) key = key | 0x02;
        if(engine::keyState[engine::kbLeft]) key = key | 0x04;
        if(engine::keyState[engine::kbRight]) key = key | 0x08;
        if(engine::keyState[engine::kbZ]) key = key | 0x10;
        if(engine::keyState[engine::kbX]) key = key | 0x20;
        if(engine::keyState[engine::kbLShift]) key = key | 0x40;
        if(engine::keyState[engine::kbC]) key = key | 0x80;
        dat_player.update(key);
    }

    void draw() {
        img_player->drawSprite(dat_player.animFrame, dat_player.x_pos + 16.f, dat_player.y_pos + 24.f);
        img_player->buffer();
        img_player->draw();
    }


    void load() {
        img_player = new SpriteSheet();
        img_player->load("./data/pl00.png", 25);
        for(int i = 0; i < 8; i++) {
            img_player->setSprite(i, i * 32, 0, 32, 48);
            img_player->setSprite(i + 8, i * 32, 48, 32, 48);
            img_player->setSprite(i + 16, i * 32, 96, 32, 48);
        }
        img_player->setSprite(24, 0, 146, 16, 12);
        
        //  center at 0,0 sprite is 32x48
        dat_player.init(608.f, 432.f);
    }

    void unload() {
        img_player->unload();
        delete img_player;
    }
}