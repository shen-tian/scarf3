#ifndef RAINBOWBLAST_H
#define RAINBOWBLAST_H

#include "Pattern.h"
#include "../State.h"

class RainbowBlast : public Pattern
{
public:
    RainbowBlast(int idx);
    void fill(CRGB *leds, long numLEDs, long t, long dt, State &state);

private:
    int paramIndex;
};

#endif