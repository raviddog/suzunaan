#include "engine/engine.hpp"
#include "state.hpp"

int main(int argv, char** args) {
    const int flags =   engine::ENGINE_INIT_FIXEDDRAWSIZE |
                        engine::ENGINE_INIT_FIXEDFPS;
    if (!engine::init("touhou suzunaan", flags, 640, 480, "./suzunaan.ini")) {
        return -1;
    }
    init();

    while(!engine::gl::quit) {
        //  update input struct
        engine::inputs();

        logic();
        draw();

        engine::flip();
    }
    engine::close();
    return 0;
}



