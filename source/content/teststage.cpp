#include "engine/engine.h"
#include "player.h"

using namespace engine;
using namespace game::content;

namespace game::teststage {

    SpriteSheet *img_player;
    player_s player;
    
    
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
    }

    void draw() {
        //  draw player bullets
        for(int i = 0; i < 42; i++) {
            if(player.bullet[i].active_frame > 0) {
                img_player->drawSprite(player.bullet[i].active_frame + 23, player.bullet[i].x_pos, player.bullet[i].y_pos, 90.f);
            }
        }

        img_player->drawSprite(player.animFrame, player.x_pos + 16.f, player.y_pos + 24.f);
        img_player->buffer();
        img_player->draw();
    }


    void load() {
        img_player = new SpriteSheet();
        img_player->load("./data/pl00.png", 28);
        for(int i = 0; i < 8; i++) {
            img_player->setSprite(i, i * 32, 0, 32, 48);
            img_player->setSprite(i + 8, i * 32, 48, 32, 48);
            img_player->setSprite(i + 16, i * 32, 96, 32, 48);
        }
        img_player->setSprite(24, 0, 146, 16, 12);
        img_player->setSprite(25, 16, 146, 16, 12);
        img_player->setSprite(26, 32, 146, 16, 12);
        img_player->setSprite(27, 48, 146, 16, 12);
        
        //  center at 0,0 sprite is 32x48
        player.init(608.f, 432.f);
    }

    void unload() {
        img_player->unload();
        delete img_player;
    }
}