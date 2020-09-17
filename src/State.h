#ifndef STATE_H
#define STATE_H

#include <stdint.h>

class State 
{
    public:

    int bgMode = 0;

    uint8_t params[5][6];

    /*
    0 is bg track
    1 - 6 are params
    ??
    */
    int selectedIdx = 0;

    bool dim = false;

    State(){}

    void incSelected();
    void decSelected();

    void incSelect();
};

#endif