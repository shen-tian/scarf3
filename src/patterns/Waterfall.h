#ifndef WATERFALL_H
#define WATERFALL_H

#include "Pattern.h"
#include "../State.h"

class Waterfall : public Pattern
{
public:
    Waterfall(int idx);
    void fill(CRGB *leds, long numLEDs, long t, long dt, State &state);

private:
    int paramIndex;
};

#endif