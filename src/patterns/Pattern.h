#ifndef PATTERN_H
#define PATTERN_H

#include <FastLED.h>

#include "../State.h"

class Pattern
{
public:
    virtual void fill(CRGB *leds, long numLEDs, long t, long dt, State &state) = 0;
    virtual const char *getLabel() = 0;
};

#endif