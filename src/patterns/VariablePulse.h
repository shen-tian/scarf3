#ifndef VARIABLEPULSE_H
#define VARIABLEPULSE_H

#include "Pattern.h"
#include "../State.h"

class VariablePulse : public Pattern
{
public:
    VariablePulse();
    void fill(CRGB *leds, long numLEDs, long t, long dt, State &state);

private:
    // return a cyclical (sine wave) value between min and max
    float cycle(float t, float period, float min, float max);

    // blend linearly between a and b, fraction=0 returns a, fraction=1 returns b
    float blend(float a, float b, float fraction);

    int brightness_to_value(float brightness, float min_brightness);
};

#endif