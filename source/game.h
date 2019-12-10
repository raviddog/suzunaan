#ifndef _GAME_H
#define _GAME_H

namespace game {
    void init();
    void load();
    void logic();
    void draw();

    namespace menu {
        void load();
        void unload();
        void logic();
        void draw();
    }

}




#endif