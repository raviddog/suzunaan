#include "engine/engine.h"
#include "player.h"
#include "bullet.h"

#include <vector>
#include <cstdlib>
#include "time.h"


using namespace engine;
using namespace game::content;

namespace game::teststage {
    const int BULLET_MAX = 20000;
    const int BULLET_HALF = 10000;

    SpriteSheet *img_player, *img_player_b, *img_bullet;
    player_s player;

    bullet_s *bullets;
    std::vector<int> *bullet_draw_order;

    int last_added_bullet;

    int getFreeBullet() {
        int j = -1;
        for(int i = 0; i < BULLET_HALF; i++) {
            if(i + last_added_bullet >= BULLET_MAX) last_added_bullet = 0;
            if(!bullets[i + last_added_bullet].active) {
                j = i + last_added_bullet;
                break;
            }
            if(!bullets[i].active) {
                j = i;
                break;
            };
            if(!bullets[i + BULLET_HALF].active) {
                j = i + BULLET_HALF;
            }
        }
        if(j != -1) last_added_bullet = j;
        return j;
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

        for(int j = 0; j < 8; j++) {
            int i = getFreeBullet();
            if(i > -1) {
                bullets[i].active = true;
                bullet_draw_order->push_back(i);
                // bullets[i].accel = 0.016f;
                // bullets[i].speed = (float)rand() / (float)RAND_MAX * 2.f - 1.f;
                bullets[i].speed = 40.f;
                bullets[i].angle = j * 45.f;
                bullets[i].x_pos = (float)(j * 80);
                bullets[i].y_pos = 360.f;
                bullets[i].instructions = nullptr;
            }
        }
    }
    int frame = 0;

    void draw() {
        //  draw player bullets
        for(int i = 0; i < 42; i++) {
            if(player.bullet[i].active_frame > 0) {
                img_player_b->drawSprite(player.bullet[i].active_frame - 1, player.bullet[i].x_pos, player.bullet[i].y_pos, 90.f);
            }
        }

        img_player_b->buffer();
        img_player_b->draw();
        
        frame += 1;
        int count = 0;
        //  draw enemy bullets
        for(int i = 0; i < bullet_draw_order->size(); i++) {
            bullets[(*bullet_draw_order)[i]].update();
            if(!bullets[(*bullet_draw_order)[i]].active) {
                bullet_draw_order->erase(bullet_draw_order->begin() + i);
            } else {
                count += 1;
                img_bullet->drawSprite(0, bullets[(*bullet_draw_order)[i]].x_pos, bullets[(*bullet_draw_order)[i]].y_pos, bullets[(*bullet_draw_order)[i]].angle);
            }
        }
        if(frame >= 60) {
            frame = 0;
            printf("bullets: %d ", count);
        }
        
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
        img_bullet->setSprite(0, 64, 16, 16, 16);

        bullets = new bullet_s[BULLET_MAX];
        for(int i = 0; i < BULLET_MAX; i++) {
            bullets[i].active = false;
        }
        last_added_bullet = 0;
        bullet_draw_order = new std::vector<int>();
    }

    void unload() {
        img_player->unload();
        img_player_b->unload();
        img_bullet->unload();
        delete img_player;
        delete img_player_b;
        delete img_bullet;
        delete bullets;
        delete bullet_draw_order;
    }
}