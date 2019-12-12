#include "engine/engine.h"
#include "player.h"
#include "bullet.h"

#include <vector>
#include <cstdlib>
#include "time.h"

using namespace engine;
using namespace game::content;

namespace game::teststage {

    SpriteSheet *img_player, *img_player_b, *img_bullet;
    player_s player;

    bullet_s *bullets;


    int getFreeBullet() {
        for(int i = 0; i < 5000; i++) {
            if(!bullets[i].active) return i;
        }
        return -1;
    }    
    
    
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
        player.update(key);

        for(int j = 0; j < 12; j++) {
            int i = getFreeBullet();
            if(i > -1) {
                bullets[i].active = true;
                bullets[i].x_accel = (float)rand() / (float)RAND_MAX * 0.2f - 0.1f;
                bullets[i].y_accel = (float)rand() / (float)RAND_MAX * 0.2f - 0.1f;
                bullets[i].x_speed = (float)rand() / (float)RAND_MAX * 4.f - 2.f;
                bullets[i].y_speed = (float)rand() / (float)RAND_MAX * 4.f - 2.f;
                bullets[i].x_pos = (float)(j * 53);
                bullets[i].y_pos = 360.f;
                bullets[i].instructions = nullptr;
            }
        }
    }

    void draw() {
        //  draw player bullets
        for(int i = 0; i < 42; i++) {
            if(player.bullet[i].active_frame > 0) {
                img_player_b->drawSprite(player.bullet[i].active_frame - 1, player.bullet[i].x_pos, player.bullet[i].y_pos, 90.f);
            }
        }

        img_player_b->buffer();
        img_player_b->draw();
        
        //  draw enemy bullets
        int count = 0;
        for(int i = 0; i < 5000; i++) {
            if(bullets[i].active) {
                bullets[i].update();
                count += 1;
                img_bullet->drawSprite(0, bullets[i].x_pos, bullets[i].y_pos);
            }
        }
        printf("%d\n", count);
        
        img_bullet->buffer();
        img_bullet->draw();

        //  draw player
        img_player->drawSprite(player.animFrame, player.x_pos + 16.f, player.y_pos + 24.f);
        img_player->buffer();
        img_player->draw();
    }


    void load() {
        srand(time(NULL));

        img_player = new SpriteSheet();
        img_player->load("./data/pl00.png", 24);
        for(int i = 0; i < 8; i++) {
            img_player->setSprite(i, i * 32, 0, 32, 48);
            img_player->setSprite(i + 8, i * 32, 48, 32, 48);
            img_player->setSprite(i + 16, i * 32, 96, 32, 48);
        }

        img_player_b = new SpriteSheet();
        img_player_b->load("./data/pl00.png", 4);
        img_player_b->setSprite(0, 0, 146, 16, 12);
        img_player_b->setSprite(1, 16, 146, 16, 12);
        img_player_b->setSprite(2, 32, 146, 16, 12);
        img_player_b->setSprite(3, 48, 146, 16, 12);
        
        //  center at 0,0 sprite is 32x48
        player.init(608.f, 432.f);

        img_bullet = new SpriteSheet();
        img_bullet->load("./data/bullet1.png", 1);
        img_bullet->setSprite(0, 64, 48, 16, 16);

        bullets = new bullet_s[5000];
        for(int i = 0; i < 5000; i++) {
            bullets[i].active = false;
        }
    }

    void unload() {
        img_player->unload();
        img_player_b->unload();
        img_bullet->unload();
        delete img_player;
        delete img_player_b;
        delete img_bullet;
    }
}