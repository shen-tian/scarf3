#ifndef OUTPUTS_H
#define OUTPUTS_H

#include <FastLED.h>

class Outputs {
    public:
    Outputs();
    void display(CRGB *leds);

    private:

    void sampleTo(CRGB *source, CRGB *target, int targetCount);

    CRGB apa102c_0[STRAND_LENGTH];
    CRGB ws2811_0[STRAND_LENGTH];
};

#endif