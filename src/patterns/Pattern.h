#ifndef PATTERN_H
#define PATTERN_H

#ifndef FASTLED_VERSION
#include <FastLED.h>
#endif

#include "../State.h"

class Pattern
{
public:
    virtual void fill(CRGB *leds, long numLEDs, long t, long dt, State &state) = 0;
    virtual const char *getLabel() = 0;
    virtual paramMetadata *getParamMetaData() = 0;
};

#endif