#include "engine/engine.hpp"
#include "state.hpp"

const int scrWidth = 640;
const int scrHeight = 480;

int main(int argv, char** args) {
    if (!engine::init("touhou suzunaan", "./suzunaan.ini")) {
        return -1;
    }
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



