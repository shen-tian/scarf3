#ifndef RAINBOWBLAST_H
#define RAINBOWBLAST_H

#include "Pattern.h"
#include "../State.h"

class RainbowBlast : public Pattern
{
public:
    RainbowBlast();
    void fill(CRGB *leds, long numLEDs, long t, long dt, State &state);
};

#endif