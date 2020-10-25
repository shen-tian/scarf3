#include "TestPattern.h"

TestPattern::TestPattern(int idx) : Pattern(idx, "Test")
{
    pMetadata = new paramMetadata[6];
    pMetadata[0] = {NORMAL, 128};
    pMetadata[1] = {OCTAVE, 96};
    pMetadata[2] = {NONE, 0};
    pMetadata[3] = {NONE, 0};
    pMetadata[4] = {NONE, 0};
    pMetadata[5] = {NONE, 0};
}

void TestPattern::fill(CRGB *leds, long numLEDs, long t, long dt, State &state)
{
    for (int i = 0; i < 128; i ++){
        uint8_t cIndex = i * 2;
        uint8_t brightness = patternParam(state, 0);
        leds[i] = ColorFromPalette(state.currentPalette, cIndex, brightness);
    }
}