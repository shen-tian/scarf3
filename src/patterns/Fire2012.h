#ifndef FIRE2012_H
#define FIRE2012_H

#include "Pattern.h"
#include "../State.h"

class Fire2012 : public Pattern
{
public:
    Fire2012();
    void fill(CRGB *leds, long numLEDs, long t, long dt, State &state);

private:
     // Array of temperature readings at each simulation cell
    byte heat[STRAND_LENGTH];
};

#endif