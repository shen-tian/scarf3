#ifndef TESTPATTERN_H
#define TESTPATTERN_H

#include "Pattern.h"
#include "../State.h"

class TestPattern : public Pattern
{
public:
    TestPattern(int idx);
    void fill(CRGB *leds, long numLEDs, long t, long dt, State &state);

private:
    int paramIndex;
};

#endif