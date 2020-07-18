#include "teststage.hpp"

#include "engine/engine.hpp"
#include "engine/debug.hpp"
#include "state.hpp"
#include "player.hpp"
#include "bullet.hpp"
#include "enemy.hpp"

#include <vector>
#include <cmath>
#include <ctime>


//  declare static member variables for reasons?
Game::player_s *Game::bullet_s::player;
Game::bullet_s* (*Game::enemy_s::getBullet)();
std::vector<Game::enemy_s*> *Game::enemy_s::enemy_draw;

namespace Game::Script {
    const int BULLET_MAX = 10000;
    float bullet_hitbox_radius_temp = 4.27f;

    engine::SpriteSheet *img_player, *img_player_b, *img_player_eff, *img_bullet, *img_enemy;
    engine::SpriteSheet *img_guibg;
    player_s player;
    bullet_s *bullets;
    std::vector<int> *bullet_draw_order;
    std::vector<int> *freebullets;
    int frames;

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
}

void Stage::logic() {
    using namespace Game::Script;

    {
        using namespace engine;
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
        //  update player
        player.update(key);

        if(keyPressed[kbEscape]) changeState(0);
    }


    //  run custom script instruction
    Game::Script::Test::teststagefunc(frames);

    //  update all bullets + prepare draw verts
    int count = 0;
    std::vector<int>::const_iterator iteratorB = bullet_draw_order->begin();
    while(iteratorB != bullet_draw_order->end()) {
        bullets[*iteratorB].update();

        //  collision
        //  check distance between player and bullet
        //  cache these values later
        if(       (bullets[*iteratorB].y_pos - player.y_pos) * (bullets[*iteratorB].y_pos - player.y_pos)
                + (bullets[*iteratorB].x_pos - player.x_pos) * (bullets[*iteratorB].x_pos - player.x_pos)
                < (bullet_hitbox_radius_temp + player.hitbox_radius) * (bullet_hitbox_radius_temp + player.hitbox_radius))
        {
            //  collided
            //  for now just remove bullet
            bullets[*iteratorB].active = false;
        }


        if(!bullets[*iteratorB].active) {
            freebullets->push_back(*iteratorB);
            iteratorB = bullet_draw_order->erase(iteratorB);
        } else {
            count += 1;
            img_bullet->drawSpriteCentered(bullets[*iteratorB].type, bullets[*iteratorB].x_pos, bullets[*iteratorB].y_pos, bullets[*iteratorB].draw_angle);
            ++iteratorB;
        }
    }

    //  update all enemies + prepare draw verts
    std::vector<Game::enemy_s*>::const_iterator iteratorE = Game::enemy_s::enemy_draw->begin();
    while(iteratorE != Game::enemy_s::enemy_draw->end()) {
        Game::enemy_s *enemy = *iteratorE;
        enemy->update();
        
        if(!enemy->active) {
            //  remove from draw vector
            //  will invalidate enemy pointer
            iteratorE = Game::enemy_s::enemy_draw->erase(iteratorE);
        } else {
            //  draw
            img_enemy->drawSpriteCentered(48, enemy->x_pos, enemy->y_pos);
            ++iteratorE;
        }
    }

    if(frames % 60 == 0) {
        engine::log_debug("bullets: %d ", count);
    }

    ++frames;
}

void Stage::draw() {
    using namespace Game::Script;

    //  buffer player bullets
    for(int i = 0; i < Game::player_s::player_bullet_max; i++) {
        if(player.bullet[i].active_frame > 0) {
            img_player_b->drawSpriteCentered(player.bullet[i].draw_frame, player.bullet[i].x_pos, player.bullet[i].y_pos, 90.f);
        }
    }

    img_guibg->draw();

    img_enemy->buffer();
    img_enemy->draw();

    img_player_b->buffer();
    img_player_b->draw();
    
    //  draw player
    img_player->drawSpriteCentered(player.animFrame, player.x_pos, player.y_pos);
    img_player->buffer();
    img_player->draw();


    img_bullet->buffer();
    img_bullet->draw();

    if(player.focused) {
        img_player_eff->drawSpriteCentered(0, player.x_pos, player.y_pos, player.focused_rotation);
        img_player_eff->drawSpriteCentered(0, player.x_pos, player.y_pos, -player.focused_rotation);
        img_player_eff->buffer();
        img_player_eff->draw();
    }
}

Stage::Stage() {
    using namespace Game::Script;
    srand(time(NULL));

    img_guibg = new engine::SpriteSheet("./data/front00.png", 4);
    img_guibg->setSprite(0, 0, 0, 32, 480);     // left
    img_guibg->setSprite(1, 32, 0, 224, 480);   // right
    img_guibg->setSprite(2, 0, 480, 384, 16);   // top
    img_guibg->setSprite(3, 0, 496, 384, 16);   // bottom

    //  stage elements are unchanging, so i only have to buffer them once
    img_guibg->drawSprite(0, 0.f, 0.f);
    img_guibg->drawSprite(1, 416.f, 0.f);
    img_guibg->drawSprite(2, 32.f, 0.f);
    img_guibg->drawSprite(3, 32.f, 464.f);
    img_guibg->buffer();

    img_player = new engine::SpriteSheet("./data/pl00.png", 24);
    for(int i = 0; i < 8; i++) {
        //  reimu
        img_player->setSprite(i, i * 32, 0, 32, 48);
        img_player->setSprite(i + 8, i * 32, 48, 32, 48);
        img_player->setSprite(i + 16, i * 32, 96, 32, 48);
    }

    img_player_b = new engine::SpriteSheet("./data/pl00.png", 4);
    //  reimu's shots
    img_player_b->setSprite(0, 0, 146, 16, 12);
    img_player_b->setSprite(1, 16, 146, 16, 12);
    img_player_b->setSprite(2, 32, 146, 16, 12);
    img_player_b->setSprite(3, 48, 146, 16, 12);

    img_player_eff = new engine::SpriteSheet("./data/eff_base.png", 1);
    //  hitbox
    img_player_eff->setSprite(0, 0, 16, 64, 64);

    img_enemy = new engine::SpriteSheet("./data/enemy.png", 60);
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

    img_bullet = new engine::SpriteSheet("./data/bullet1.png", 192, BULLET_MAX);
    for(int y = 0; y < 12; y++) {
        for(int x = 0; x < 16; x++) {
            img_bullet->setSprite(y * 16 + x, x * 16, y * 16, 16, 16);
        }
    }

    bullets = new Game::bullet_s[BULLET_MAX];
    freebullets = new std::vector<int>();
    freebullets->reserve(BULLET_MAX);
    for(int i = 0; i < BULLET_MAX; ++i) {
        freebullets->push_back(i);
    }
    bullet_draw_order = new std::vector<int>();
    frames = 0;

    Game::enemy_s::enemy_draw = new std::vector<Game::enemy_s*>();
    Game::enemy_s::getBullet = Game::Script::getFreeBulletPointer;
    Game::bullet_bounds_x = -32.f;
    Game::bullet_bounds_xmax = 672.f;
    Game::bullet_bounds_y = -32.f;
    Game::bullet_bounds_ymax = 512.f;
    Game::bullet_s::player = &player;

    Game::Script::Test::teststageload();
}

Stage::~Stage() {
    using namespace Game::Script;
    Game::Script::Test::teststageunload();
    delete img_player;
    delete img_player_b;
    delete img_player_eff;
    delete img_bullet;
    delete[] bullets;
    delete bullet_draw_order;
    delete img_guibg;
    delete Game::enemy_s::enemy_draw;
}