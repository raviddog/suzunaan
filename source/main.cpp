#define SDL_MAIN_HANDLED

#include "engine/engine.hpp"
#include "game.hpp"

const int scrWidth = 640;
const int scrHeight = 480;

int main(int argv, char** args) {
    // engine::init("test", false, 1600, 900, 640, 480);
    engine::init("test", 0, 640, 480);

    // freopen("./test.log", "w", stdout);
    
    uint32_t ticks[4] = {0};
    // 0 - frame counter
    // 1 - draw time ticks
    // 2 - fps timer
    // 3 - fps timer
    ticks[2] = SDL_GetTicks();

    game::init();

    while(!engine::quit) {
        ticks[1] = SDL_GetTicks();
        engine::inputs();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(engine::keyState[engine::kbO] || engine::keyPressed[engine::kbP]) {
        }
        game::logic();

        game::draw();
        


        SDL_GL_SwapWindow(engine::gl::window);
        // fps
        ticks[0] += 1;
        if(SDL_GetTicks() > ticks[2] + 1000) {
            // printf("frame time: %dms, ", SDL_GetTicks() - ticks[1]);
            // printf("fps: %d\n", ticks[0]);
            ticks[0] = 0;
            ticks[2] = SDL_GetTicks();
            
            // other debug messages
            // printf("sprites drawn: %d ", count);
            // printf("\n");
        }
    }
    engine::close();
    return 0;
}



