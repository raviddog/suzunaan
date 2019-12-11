#include "engine/engine.h"
#include "game.h"
#include "glm/gtx/string_cast.hpp"

const int scrWidth = 640;
const int scrHeight = 480;


int main(int args, char** argv) {
    engine::init("test", scrWidth, scrHeight);

    //freopen("./test.log", "w", stdout);
    
    uint32_t ticks[4] = {0};
    // 0 - frame counter
    // 1 - draw time ticks
    // 2 - fps timer
    // 3 - fps timer
    ticks[2] = SDL_GetTicks();

    game::init();


int animationFrame = 0;
    int animationDelay = 0;
    engine::SpriteSheet *s = new engine::SpriteSheet();
    s->load("./data/pl00.png", 24);
    for(int i = 0; i < 8; i++) {
        s->setSprite(i, i * 32, 0, 32, 46);
        s->setSprite(i + 8, i * 32, 46, 32, 46);
        s->setSprite(i + 16, i * 32, 92, 32, 46);
    }

    engine::SpriteSheet *m = new engine::SpriteSheet();
    m->load("./data/missing.png", 1);
    m->setSprite(0, 0, 0, 640, 480);
    m->drawSprite(0, 320.f, 240.f);
    m->buffer();

    while(!engine::quit) {
        ticks[1] = SDL_GetTicks();
        engine::inputs();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m->draw();

        game::logic();
        game::draw();


        animationDelay += 1;
        if(animationDelay >= 4) {
            animationDelay = 0;
            animationFrame += 1;
        }

        if(animationFrame >= 8) {
            animationFrame = 0;
        }
        s->drawSprite(animationFrame, 0.f, 0.f);
        s->drawSprite(animationFrame, 320.f, 240.f);
        // s->drawSprite(animationFrame, 320.f, 240.f, 0.7853982f);
        s->drawSprite(animationFrame, 290.f, 240.f, 0.7853982f);
        s->drawSprite(animationFrame, 352.f, 240.f, 1.047198f);
        s->drawSprite(animationFrame, 400.f, 240.f, 4.712389f);
        s->drawSprite(animationFrame, 432.f, 240.f, 6.283185f);

        // s->drawSprite(animationFrame, 320.f, 240.f, 270.f);
        // s->drawSprite(animationFrame, 320.f, 240.f, 180.f);
        // s->drawSprite(animationFrame, 320.f, 240.f, 90.f);
        // s->drawSprite(animationFrame, 320.f, 240.f, 0.f);


        s->buffer();
        s->draw();
        


        // fps
        ticks[0] += 1;
        if(SDL_GetTicks() > ticks[2] + 1000) {
            // draw time
            //printf("draw time: %dms, ", SDL_GetTicks() - ticks[1]);
            
            // fps
            //printf("fps: %d\n", ticks[0]);
            ticks[0] = 0;
            ticks[2] = SDL_GetTicks();
            
            // other debug messages

            //printf("\n");
        }

        
        SDL_GL_SwapWindow(engine::gl::window);
        

    }
    engine::close();
    return 0;
}



