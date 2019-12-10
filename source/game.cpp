#include "game.h"
#include "engine/engine.h"

namespace game {
    int curState;
    //  0 = menu

    void changeState(int newState) {
        switch(curState) {
            case 0:
                menu::unload();
                break;
        }
        curState = newState;
        switch(curState) {
            case 0:
                menu::load();
                break;
        }
    }

    void logic() {
        switch(curState) {
            case 0:
                menu::logic();
                break;
            default:
                break;
        }
    }

    void draw() {
        switch(curState) {
            case 0:
                menu::draw();
                break;
            default:
                break;
        }
    }


    void init() {
        curState = 0;
        menu::load();
    }
}