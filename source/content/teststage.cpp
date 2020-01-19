#include "engine/engine.hpp"
#include "../game.hpp"
#include "player.hpp"
#include "bullet.hpp"
#include "enemy.hpp"
#include "script.hpp"

#include <vector>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <memory>


using namespace engine;
using namespace game::content;

namespace game::teststage {
    const int BULLET_MAX = 10000;
    float bullet_hitbox_radius_temp = 4.27f;

    SpriteSheet *img_player, *img_player_b, *img_player_eff, *img_bullet, *img_enemy;
    player_s player;

    bullet_s *bullets;
    std::vector<int> *bullet_draw_order;
    std::vector<int> *freebullets;
    int last_added_bullet;
    int frames;

    
    bullet_script *toziko, *miko, *boot;

    std::unordered_map<uint32_t, std::shared_ptr<bullet_script>> *bullet_scripts;
    enemy_script *enemyscript;

    enemy_s *testenemy;

    int getFreeBullet() {
        if(freebullets->empty()) {
            return -1;
        } else {
            int j = freebullets->back();
            bullet_draw_order->push_back(j);
            freebullets->pop_back();
            bullets[j].reset();
            bullets[j].active = true;
            return j;
        }
    }

    bullet_s* getFreeBulletPointer() {
        int j = getFreeBullet();
        if(j == -1) {
            return nullptr;
        } else {
            return &(bullets[j]);
        }
    }

    template <class T>
    T between(T min, T max) {
        return (max - min) * (float)rand() / (float)RAND_MAX + min;
    }

    
    void logic() {
		bool quitToMenu = false;
        //  c shift x z right left down up
        unsigned char key = 0x00;
        if(keyState[kbUp]) key = key | 0x01;
        if(keyState[kbDown]) key = key | 0x02;
        if(keyState[kbLeft]) key = key | 0x04;
        if(keyState[kbRight]) key = key | 0x08;
        if(keyState[kbZ]) key = key | 0x10;
        if(keyState[kbX]) key = key | 0x20;
        if(keyState[kbLShift]) key = key | 0x40;
        if(keyState[kbC]) key = key | 0x80;
        player.update(key);

		if(keyPressed[kbEscape]) quitToMenu = true;
        if(keyPressed[kbP]) SpriteSheet::useShaderNormal();
        if(keyPressed[kbO]) SpriteSheet::useShaderInvert();

        frames++;

        if(frames == 1) {
            // testenemy->activate();
        }
        testenemy->update();


        

        if(frames % 3 == 0) {
            // int i = getFreeBullet();
            // if(i > -1) {
            //     bullets[i].type = 24;
            //     bullets[i].instructions = boot;
            //     bullets[i].y_pos = 120.f;
            //     bullets[i].x_pos = between<float>(0.f, 640.f);
            //     bullets[i].type = 168;
                
            // }

            //  bowap
            for(int j = 0; j < 12; j++) {
                int i = getFreeBullet();
                if(i > -1) {
                    bullets[i].type = 48 + j % 16;
                    bullets[i].active = true;
                    bullets[i].accel = 0.f;
                    bullets[i].x_pos = 320.f;
                    bullets[i].y_pos = 120.f;
                    bullets[i].angle = 1080.f * sin((double)frames * 3.14159265/180) + j * 45.f;
                    bullets[i].angle = -(float)frames / 10.f + j * 30.f;
                    bullets[i].speed = 0.f;
                    bullets[i].instructions = toziko;
                }
            }
        }

        // for(int j = 0; j < 64; j++) {
        //     int i = getFreeBullet();
        //     if(i > -1) {
        //         bullets[i].type = 48 + j % 16;
        //         bullets[i].active = true;
        //         bullets[i].frames = 0;
        //         bullets[i].accel = 0.f;
        //         bullets[i].x_pos = j * 10.f;
        //         bullets[i].y_pos = 360.f;
        //         bullets[i].angle = between(0.f, 360.f);
        //         bullets[i].speed = 3.f;
        //         bullets[i].instructions = test;
        //     }
        // }


        int count = 0;
        //  update and buffer all bullets

        std::vector<int>::const_iterator iterator;
        iterator = bullet_draw_order->begin();


        while(iterator != bullet_draw_order->end()) {
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
                freebullets->push_back(*iterator);
                iterator = bullet_draw_order->erase(iterator);
            } else {
                count += 1;
                img_bullet->drawSprite(bullets[*iterator].type, bullets[*iterator].x_pos, bullets[*iterator].y_pos, bullets[*iterator].draw_angle);
                ++iterator;
            }
        }

        
        //  update enemy
        // img_enemy->drawSprite(48, testenemy->x_pos, testenemy->y_pos);


        if(frames % 60 == 0) {
            // printf("bullets: %d ", count);
        }

		if(quitToMenu) game::changeState(0);
    }
    

    void draw() {
        //  draw player bullets
        for(int i = 0; i < 42; i++) {
            if(player.bullet[i].active_frame > 0) {
                img_player_b->drawSprite(player.bullet[i].draw_frame, player.bullet[i].x_pos, player.bullet[i].y_pos, 90.f);
            }
        }

        img_enemy->buffer();
        img_enemy->draw();

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

        img_player = new SpriteSheet("./data/pl00.png", 24);
        for(int i = 0; i < 8; i++) {
            //  reimu
            img_player->setSprite(i, i * 32, 0, 32, 48);
            img_player->setSprite(i + 8, i * 32, 48, 32, 48);
            img_player->setSprite(i + 16, i * 32, 96, 32, 48);
        }

        img_player_b = new SpriteSheet("./data/pl00.png", 4);
        //  reimu's shots
        img_player_b->setSprite(0, 0, 146, 16, 12);
        img_player_b->setSprite(1, 16, 146, 16, 12);
        img_player_b->setSprite(2, 32, 146, 16, 12);
        img_player_b->setSprite(3, 48, 146, 16, 12);

        img_player_eff = new SpriteSheet("./data/eff_base.png", 1);
        //  hitbox
        img_player_eff->setSprite(0, 0, 16, 64, 64);

        img_enemy = new SpriteSheet("./data/enemy.png", 60);
        //  small fairies
        for(int y = 0; y < 4; y++) {
            for(int x = 0; x < 12; x++) {
                img_enemy->setSprite(y * 12 + x, x * 32, 256 + y * 32, 32, 32);
            }
        }
        //  sunflower fairy
        for(int i = 0; i < 8; i++) {
            img_enemy->setSprite(48 + i, i * 64, 384, 64, 64);
        }
        
        //  center at 0,0 sprite is 32x48
        player.init(16.f, 24.f, 624.f, 456.f);
        player.hitbox_radius = 1.f;

        img_bullet = new SpriteSheet("./data/bullet1.png", 192, BULLET_MAX);
        for(int y = 0; y < 12; y++) {
            for(int x = 0; x < 16; x++) {
                img_bullet->setSprite(y * 16 + x, x * 16, y * 16, 16, 16);
            }
        }



        bullets = new bullet_s[BULLET_MAX + 1];
        freebullets = new std::vector<int>();
        freebullets->reserve(BULLET_MAX);
        for(int i = 0; i < BULLET_MAX; i++) {
            freebullets->push_back(i);
        }
        last_added_bullet = 0;
        bullet_draw_order = new std::vector<int>();
        frames = 0;

        getBullet = getFreeBulletPointer;
        bullet_bounds_x = -32.f;
        bullet_bounds_xmax = 672.f;
        bullet_bounds_y = -32.f;
        bullet_bounds_ymax = 512.f;

        bullet_scripts = new std::unordered_map<uint32_t, std::shared_ptr<bullet_script>>();
        std::shared_ptr<bullet_script> temp(loadScriptBullet("./script/miko.txt"));
        bullet_scripts->insert({1, temp});

        content::bullet_scripts = teststage::bullet_scripts;
        
        toziko = loadScriptBullet("./script/toziko.txt");
        miko = loadScriptBullet("./script/miko.txt");
        boot = loadScriptBullet("./script/boot_on_head.txt");
        enemyscript = loadScriptEnemy("./script/testenemy.txt");

        testenemy = new enemy_s();
        testenemy->x_pos = 320.f;
        testenemy->y_pos = 20.f;
        testenemy->instructions = enemyscript;

    }

    void unload() {
        delete img_player;
        delete img_player_b;
        delete img_player_eff;
        delete img_bullet;
        delete[] bullets;
        delete bullet_draw_order;
        delete toziko;
        delete miko;
    }
}