#ifndef CLOUD_H
#define CLOUD_H

#include "Pattern.h"
#include "../State.h"

class Cloud : public Pattern
{
public:
    Cloud();
    void fill(CRGB *leds, long numLEDs, long t, long dt, State &state);

private:
    uint8_t fractalNoise(uint16_t x, uint16_t y, int octave);
    long pos;
};

#endif