#ifndef _ENGINE_INPUT_H
#define _ENGINE_INPUT_H


#include "libs/ini.h"



namespace engine {
    //  set game controls here
    enum inputKeys {
        inputUp, inputDown, inputLeft, inputRight,
        inputFire, inputFocus, inputBomb, inputPause,
        inputQuit, inputRestart, inputSkip,



        controlSize
    };

    void init_input(ini_t *ini);
    void inputs();


    bool checkKey(int key);
    bool checkKeyPressed(int key);
    float checkKeyAxis(int key);


    void mouseCapture();
    void mouseRelease();
}


#endif