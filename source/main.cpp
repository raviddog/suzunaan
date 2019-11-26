#include "engine/engine.h"

const int scrWidth = 640;
const int scrHeight = 480;


int main(int args, char** argv) {

    engine::init("test", scrWidth, scrHeight);

    

    while(!engine::quit) {
        engine::inputs();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        SDL_GL_SwapWindow(engine::gl::window);
    }

    return 0;
}



