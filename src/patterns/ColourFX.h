#ifndef COLOUR_FX_H
#define COLOUR_FX_H

#include "State.h"

#include "Pattern.h"
#include "../State.h"

class ColourFX : public Pattern
{
public:
    ColourFX();
    void fill(CRGB *leds, long numLEDs, long t, long dt, State &state);
};

#endif