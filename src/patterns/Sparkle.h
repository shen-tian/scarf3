#ifndef SPARKLE_H
#define SPARKLE_H

#include "Pattern.h"
#include "../State.h"

class Sparkle : public Pattern
{
public:
    Sparkle(int idx);
    void fill(CRGB *leds, long numLEDs, long t, long dt, State &state);
};

#endif