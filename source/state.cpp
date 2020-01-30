#include "state.hpp"

static int newState;

State *currentState;
//  0 = menu
//  1 = teststage

void changeState(int state) {
    newState = state;
}

//  can probably inline these or merge this entire file with main
void logic() {
    currentState->logic();
    //  check for a request to change stage after logic is done
    if(newState > -1) {
        delete currentState;
        switch(newState) {
            default:
            case 0:
                currentState = new Menu();
                break;
            case 1:
                currentState = new Stage();
                break;
        }
        newState = -1;

        //  run logic again to load up the draw buffers on the new state
        //  there's probably a smarter way to do this
        currentState->logic();
    }
}

void draw() {
    currentState->draw();
}


void init() {
    currentState = new Menu();
}