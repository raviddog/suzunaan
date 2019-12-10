#include "engine/engine.h"

const int scrWidth = 640;
const int scrHeight = 480;


int main(int args, char** argv) {
    engine::init("test", scrWidth, scrHeight);

    //freopen("./test.log", "w", stdout);

    engine::SpriteSheet *s = new engine::SpriteSheet();
    s->load("./data/pl00.png", 24);
    for(int i = 0; i < 8; i++) {
        s->setSprite(i, i * 32, 0, 32, 46);
        s->setSprite(i + 8, i * 32, 46, 32, 46);
        s->setSprite(i + 16, i * 32, 92, 32, 46);
    }

    engine::SpriteSheet *s2 = new engine::SpriteSheet();
    s2->load("./data/eff_base.png", 2);
    s2->setSprite(0, 0, 208, 16, 16);
    s2->setSprite(1, 16, 208, 16, 16);
    
    uint32_t ticks[4] = {0};
    // 0 - frame counter
    // 1 - draw time ticks
    // 2 - fps timer
    // 3 - fps timer
    ticks[2] = SDL_GetTicks();

    int animationFrame = 0;
    int animationDelay = 0;

    float charX = 200.f, charY = 200.f;
    float moveSpeed = 4.f;

    int c2 = 0;

    for(int i = 0; i < 640; i += 2) {
        for(int j = 0; j < 480; j += 2) {
            s2->drawSprite(1, (float)i, (float)j);
            c2 += 1;
        }
    }

    s2->buffer();

    while(!engine::quit) {
        ticks[1] = SDL_GetTicks();
        engine::inputs();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int count = c2;
        
        animationDelay += 1;
        if(animationDelay >= 4) {
            animationDelay = 0;
            animationFrame += 1;
            if(animationFrame >= 8) {
                animationFrame = 0;
            }

            for(int i = 0; i < 640; i += 10) {
                for(int j = 0; j < 480; j += 10) {
                    s->drawSprite(animationFrame, (float)i, (float)j);
                    count += 1;
                }
            }
            s->buffer();
        }


        s2->draw();
        s->draw();





        if(engine::keyState[engine::kbLeft]) {
            charX -= moveSpeed;
        }
        if(engine::keyState[engine::kbRight]) {
            charX += moveSpeed;
        }
        if(engine::keyState[engine::kbUp]) {
            charY -= moveSpeed;
        }
        if(engine::keyState[engine::kbDown]) {
            charY += moveSpeed;
        }

        // fps
        ticks[0] += 1;
        // ticks[2] = SDL_GetTicks();
        if(SDL_GetTicks() > ticks[2] + 1000) {
            // draw time
            printf("draw time: %dms, ", SDL_GetTicks() - ticks[1]);
            
            // fps
            printf("fps: %d\n", ticks[0]);
            ticks[0] = 0;
            ticks[2] = SDL_GetTicks();
            
            // other debug messages
            printf("sprites drawn: %d ", count);

            printf("\n");
        }

        
        SDL_GL_SwapWindow(engine::gl::window);
        

    }
    engine::close();
    return 0;
}



