#ifndef _GAME_H
#define _GAME_H

namespace game {
    void init();
    void changeState(int);
    void logic();
    void draw();

    namespace menu {
        void load();
        void unload();
        void logic();
        void draw();
    }

    namespace teststage {
        void load();
        void unload();
        void logic();
        void draw();
    }

}




#endif