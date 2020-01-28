#include "state.hpp"
#include "engine/engine.hpp"

//  migrate these to class members later
static engine::SpriteSheet *img_base_options;
static int opt_selected, opt_tree;
static bool draw_changed;

void Menu::logic() {
    if(opt_tree == 0) {
        if(engine::keyPressed[engine::kbUp]) {
            opt_selected -= 1;
            draw_changed = true;
        }
        if(engine::keyPressed[engine::kbDown]) {
            opt_selected += 1;
            draw_changed = true;
        }

        if(opt_selected < 0) {
            opt_selected = 1;
        } else if(opt_selected > 1) {
            opt_selected = 0;
        }

        if(draw_changed) {
            //  rebuffer
            img_base_options->drawSprite(opt_selected + 2, 320.f, 240.f - (opt_selected * 64.f));
            img_base_options->drawSprite(0, 320.f, 240.f);
            img_base_options->drawSprite(1, 320.f, 176.f);
            
            
            img_base_options->buffer();
            draw_changed = false;
        }


        if(engine::keyPressed[engine::kbZ]) {
            if(opt_selected == 0) {
                changeState(1);
            } else if(opt_selected == 1) {
                engine::quit = true;
            }
        }
    }
}

void Menu::draw() {
    if(opt_tree == 0) {
        img_base_options->draw();
    }
}


Menu::Menu() {
    img_base_options = new engine::SpriteSheet("./data/menu.png", 4);
    img_base_options->setSprite(0, 0, 0, 160, 64);
    img_base_options->setSprite(1, 0, 64, 160, 64);
    img_base_options->setSprite(2, 160, 0, 160, 64);
    img_base_options->setSprite(3, 160, 64, 160, 64);

    opt_selected = 0;
    opt_tree = 0;
    draw_changed = true;
}

Menu::~Menu() {
    delete img_base_options;
}