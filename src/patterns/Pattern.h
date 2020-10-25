#ifndef PATTERN_H
#define PATTERN_H

#ifndef FASTLED_VERSION
#include <FastLED.h>
#endif

#include "../State.h"

class Pattern
{
protected:
    paramMetadata *pMetadata;
    uint8_t patternParam(State &state, int idx);
    float octave(State &state, int idx);

public:
    Pattern(const char* labelStr);
    virtual void fill(CRGB *leds, long numLEDs, long t, long dt, State &state) = 0;
    const char *getLabel();
    paramMetadata *getParamMetaData();
    void setParamIdx(int idx);
private:
    int paramIndex;
    const char* label;
};

#endif