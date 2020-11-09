#ifndef DELAY_FX_H
#define DELAY_FX_H

#include "State.h"

#include "Pattern.h"
#include "../State.h"

class DelayFX : public Pattern
{
public:
    DelayFX();
    void fill(CRGB *leds, long numLEDs, long t, long dt, State &state);
private:
    CRGB buffer[STRAND_LENGTH];
};

#endif