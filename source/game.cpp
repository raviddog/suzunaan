#include "game.h"

namespace game {
    int curState;
    //  0 = menu
    //  1 = teststage

    void changeState(int newState) {
        switch(curState) {
            case 0:
                menu::unload();
                break;
            case 1:
                teststage::unload();
                break;
        }
        curState = newState;
        switch(curState) {
            case 0:
                menu::load();
                break;
            case 1:
                teststage::load();
                break;
        }
    }

    void logic() {
        switch(curState) {
            case 0:
                menu::logic();
                break;
            case 1:
                teststage::logic();
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
            case 1:
                teststage::draw();
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