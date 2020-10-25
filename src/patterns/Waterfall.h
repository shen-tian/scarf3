#ifndef WATERFALL_H
#define WATERFALL_H

#include "Pattern.h"
#include "../State.h"

class Waterfall : public Pattern
{
public:
    Waterfall();
    void fill(CRGB *leds, long numLEDs, long t, long dt, State &state);
};

#endif