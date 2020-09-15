#ifndef STATE_H
#define STATE_H

class State 
{
    public:

    int bgMode = 0;
    int fooParam = 0;

    int selectedIdx = 0;

    bool dim = false;

    State(){}

    void incMode();
    void decMode();

    void incSelect();
};

#endif