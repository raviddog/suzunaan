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
    void logic() override;
    void draw() override;
};

//  should i put all states here or just include a bunch of files
//  put them here for now

class Stage : public State {
    public:
        Stage();
        ~Stage();
        void logic() override;
        void draw() override;
};

class Test3D : public State {
    public:
        Test3D();
        ~Test3D();
        void logic() override;
        void draw() override;
};




#endif