#ifndef STATE_H
#define STATE_H

#include <stdint.h>

class State 
{
    public:

    int bgMode = 0;

    uint8_t patternParams[5][6];

    uint8_t globalParams[6];
    /*
    0 is bg track
    1 - 6 are params
    ??
    */
    int selectedIdx = 0;

    int selectedLayer = 0;

    bool dim = false;

    State(){}

    void incSelected();
    void decSelected();
    void zeroSelected();

    void cycleParam();
    void cycleLayer();

    uint8_t visibleParam(int idx);
};

#endif