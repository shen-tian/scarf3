#ifndef SCARF_H
#define SCARF_H

#include "Pattern.h"
#include "../State.h"

#define HUE_START .5
#define HUE_END .75
#define SATURATION .8

/**
    Pattern definition. The program cycles through a range on the wheel, and
    back again. This defines the boundaries. Note that wraparound for the full
    rainbow is not enabled. Would take special case code.
*/

class Scarf : public Pattern
{
public:
    Scarf(int idx);
    void fill(CRGB *leds, long numLEDs, long t, long dt, State &state); 

private:
    long pos;
    int paramIndex;
    byte getClock(unsigned long mil, byte rate);
    byte modDist(byte x, byte y);
};

#endif