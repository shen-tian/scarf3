#ifndef CLOUD_H
#define CLOUD_H

#include "Pattern.h"
#include "../State.h"

class Cloud : public Pattern
{
public:
    Cloud(int idx);
    void fill(CRGB *leds, long numLEDs, long t, long dt, State &state);

private:
    long pos;
};

#endif