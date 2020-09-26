#ifndef PATTERN_H
#define PATTERN_H

#ifndef FASTLED_VERSION
#include <FastLED.h>
#endif

#include "../State.h"

class Pattern
{
protected:
    const char* label;
    paramMetadata *pMetadata;
public:
    virtual void fill(CRGB *leds, long numLEDs, long t, long dt, State &state) = 0;
    const char *getLabel();
    paramMetadata *getParamMetaData();
};

#endif