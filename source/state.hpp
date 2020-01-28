#ifndef _STATE_H
#define _STATE_H

void init();
void changeState(int);
void logic();
void draw();

class State {
    public:
        virtual ~State() {};
        virtual void logic() =0;
        virtual void draw() =0;
};

class Menu : public State {
    public:
    Menu();
    ~Menu();
    void logic();
    void draw();
};

//  should i put all states here or just include a bunch of files
//  put them here for now

class teststage : public State {
    public:
        teststage();
        ~teststage();
        void logic();
        void draw();
};




#endif