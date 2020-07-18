#define SDL_MAIN_HANDLED

#include "engine/engine.hpp"
#include "state.hpp"

const int scrWidth = 640;
const int scrHeight = 480;

int main(int argv, char** args) {
    // engine::init("test", 2, 1600, 900, 640, 480);
    engine::init("touhou suzunaan", 1, true, 640, 480);
    // engine::init("test", 0, true, 640, 480);

    // freopen("./test.log", "w", stdout);

    init();

    while(!engine::quit) {
        //  update input struct
        engine::inputs();

        logic();
        draw();

        engine::flip();
    }
    engine::close();
    return 0;
}



