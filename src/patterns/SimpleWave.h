#ifndef SIMPLEWAVE_H
#define SIMPLEWAVE_H

#include "Pattern.h"
#include "../State.h"

class SimpleWave : public Pattern
{
public:
    SimpleWave();
    void fill(CRGB *leds, long numLEDs, long t, long dt, State &state);
};

#endif 