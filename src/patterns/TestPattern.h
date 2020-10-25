#ifndef TESTPATTERN_H
#define TESTPATTERN_H

#include "Pattern.h"
#include "../State.h"

class TestPattern : public Pattern
{
public:
    TestPattern();
    void fill(CRGB *leds, long numLEDs, long t, long dt, State &state);
};

#endif