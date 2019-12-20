#include "engine/engine.h"
#include "player.h"
#include "bullet.h"
#include "enemy.h"

#include <vector>
#include <list>
#include <cstdlib>
#include "math.h"
#include "time.h"
#include "script.h"


using namespace engine;
using namespace game::content;

namespace game::teststage {
    const int BULLET_MAX = 80000;
    float bullet_hitbox_radius_temp = 4.27f;

    SpriteSheet *img_player, *img_player_b, *img_player_eff, *img_bullet;
    player_s player;

    bullet_s *bullets;
    std::list<int> *bullet_draw_order;

    int last_added_bullet;
    int frames;

    std::unordered_map<int, script_instruction_bullet*> *test;

    int getFreeBullet() {
        int j = -1;
        for(int i = 0; i < BULLET_MAX; i++) {
            if(i + last_added_bullet >= BULLET_MAX) last_added_bullet = 0;
            if(!bullets[i + last_added_bullet].active) {
                j = i + last_added_bullet;
                break;
            }
        }
        if(j != -1) last_added_bullet = j;
        return j;
    }

    bullet_s* getFreeBulletPointer() {
        int j = -1;
        for(int i = 0; i < BULLET_MAX; i++) {
            if(i + last_added_bullet >= BULLET_MAX) last_added_bullet = 0;
            if(!bullets[i + last_added_bullet].active) {
                j = i + last_added_bullet;
                break;
            }
        }
        if(j != -1) last_added_bullet = j;
        return &bullets[j];
    }

    template <class T>
    T between(T min, T max) {
        return (max - min) * (float)rand() / (float)RAND_MAX + min;
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

        frames++;

        // if(frames % 3 == 0) {
        //     for(int j = 0; j < 24; j++) {
        //         int i = getFreeBullet();
        //         if(i > -1) {
        //             bullets[i].reset();
        //             bullet_draw_order->push_front(i);
        //             bullets[i].type = 48 + j % 16;
        //             bullets[i].active = true;
        //             bullets[i].frames = 0;
        //             bullets[i].accel = 0.016f;
        //             bullets[i].x_pos = 320.f;
        //             bullets[i].y_pos = 360.f;
        //             bullets[i].angle = 360.f * sin((double)frames * 3.14159265/180) + j * 15.f;
        //             bullets[i].speed = 3.f;
        //             bullets[i].instructions = test;
        //         }
        //     }
        // }

        for(int j = 0; j < 64; j++) {
            int i = getFreeBullet();
            if(i > -1) {
                bullets[i].reset();
                bullet_draw_order->push_front(i);
                bullets[i].type = 48 + j % 16;
                bullets[i].active = true;
                bullets[i].frames = 0;
                bullets[i].accel = 0.f;
                bullets[i].x_pos = j * 10.f;
                bullets[i].y_pos = 360.f;
                bullets[i].angle = between(0.f, 360.f);
                bullets[i].speed = 3.f;
                bullets[i].instructions = test;
            }
        }


        int count = 0;
        //  update and buffer all bullets

        std::list<int>::const_iterator iterator, end;
        iterator = bullet_draw_order->begin();
        end = bullet_draw_order->end();

        while(iterator != end) {
            bullets[*iterator].update();

            //  initial collision
            //  check if y-axis is near player
            if(std::abs(bullets[*iterator].y_pos - player.y_pos) < bullet_hitbox_radius_temp + player.hitbox_radius) {
                //check distance between player and bullet
                if((bullets[*iterator].y_pos - player.y_pos) * (bullets[*iterator].y_pos - player.y_pos)
                     + (bullets[*iterator].x_pos - player.x_pos) * (bullets[*iterator].x_pos - player.x_pos)
                     < (bullet_hitbox_radius_temp + player.hitbox_radius) * (bullet_hitbox_radius_temp + player.hitbox_radius))
                {
                    //  collided
                    //  for now just remove bullet
                    bullets[*iterator].active = false;
                }
            }


            if(!bullets[*iterator].active) {
                iterator = bullet_draw_order->erase(iterator);
            } else {
                count += 1;
                img_bullet->drawSprite(bullets[*iterator].type, bullets[*iterator].x_pos, bullets[*iterator].y_pos, bullets[*iterator].draw_angle);
                iterator++;
            }
        }


        if(frames % 60 == 0) {
            printf("bullets: %d ", count);
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
        
        //  draw player
        img_player->drawSprite(player.animFrame, player.x_pos, player.y_pos);
        img_player->buffer();
        img_player->draw();


        img_bullet->buffer();
        img_bullet->draw();

        if(player.focused) {
            img_player_eff->drawSprite(0, player.x_pos, player.y_pos, player.focused_rotation);
            img_player_eff->drawSprite(0, player.x_pos, player.y_pos, -player.focused_rotation);
            img_player_eff->buffer();
            img_player_eff->draw();
        }
    }


    void load() {
        srand(time(NULL));
        script_init();

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

        img_player_eff = new SpriteSheet();
        img_player_eff->load("./data/eff_base.png", 1);
        img_player_eff->setSprite(0, 0, 16, 64, 64);
        
        //  center at 0,0 sprite is 32x48
        player.init(16.f, 24.f, 624.f, 456.f);
        player.hitbox_radius = 1.f;

        img_bullet = new SpriteSheet();
        img_bullet->load("./data/bullet1.png", 192, BULLET_MAX);
        // img_bullet->load("./data/bullet1.png", 192);
        for(int y = 0; y < 12; y++) {
            for(int x = 0; x < 16; x++) {
                img_bullet->setSprite(y * 16 + x, x * 16, y * 16, 16, 16);
            }
        }



        bullets = new bullet_s[BULLET_MAX + 1];
        for(int i = 0; i < BULLET_MAX; i++) {
            bullets[i].active = false;
        }
        last_added_bullet = 0;
        bullet_draw_order = new std::list<int>();
        frames = 0;

        test = loadScriptBullet("./script/testenemy1.str");
        getBullet = getFreeBulletPointer;
        bullet_bounds_x = -32.f;
        bullet_bounds_xmax = 672.f;
        bullet_bounds_y = -32.f;
        bullet_bounds_ymax = 512.f;

    }

    void unload() {
        img_player->unload();
        img_player_b->unload();
        img_player_eff->unload();
        img_bullet->unload();
        delete img_player;
        delete img_player_b;
        delete img_player_eff;
        delete img_bullet;
        delete bullets;
        delete bullet_draw_order;
        delete test;
    }
}