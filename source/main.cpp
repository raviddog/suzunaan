#define SDL_MAIN_HANDLED

#include "engine/engine.hpp"
#include "state.hpp"

const int scrWidth = 640;
const int scrHeight = 480;

int main(int argv, char** args) {
    engine::init("touhou suzunaan", "./suzunaan.ini");
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



