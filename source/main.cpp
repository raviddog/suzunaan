#include "engine/engine.h"

const int scrWidth = 640;
const int scrHeight = 480;

float vert[] = {
    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 1.0f, 1.0f
};

uint32_t ind[] = {
    0, 1, 2,
    0, 1, 3
};
int main(int args, char** argv) {
    engine::init("test", scrWidth, scrHeight);

    engine::SpriteSheet *s = new engine::SpriteSheet();
    s->load("./data/pl00.png", 2);
    s->setSprite(0, 0, 0, 32, 46);
    s->setSprite(1, 224, 92, 32, 46);
    
    uint32_t ticks[4] = {0};
    ticks[0] = SDL_GetTicks();

    while(!engine::quit) {
        ticks[2] = SDL_GetTicks();
        engine::inputs();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        s->drawSprite(0, 10, 10);
        s->drawSprite(0, 200, 200);
        s->drawSprite(1, 0, 0);
        s->drawSprite(1, 2, 1);
        s->drawSprite(1, 400, 0);
        s->draw();

        ticks[3] = SDL_GetTicks();
        printf("draw time: %dms, ", ticks[3] - ticks[2]);
        SDL_GL_SwapWindow(engine::gl::window);
        ticks[1] = SDL_GetTicks();
        printf("frame time: %dms\n", ticks[1] - ticks[0]);
        ticks[0] = ticks[1];
    }
    engine::close();
    return 0;
}



