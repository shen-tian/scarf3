#include "SimpleWave.h"

SimpleWave::SimpleWave(int idx){
    paramIndex = idx;
    label = "Wave";
    pMetadata = new paramMetadata[6];
    pMetadata[0] = {NORMAL, 128};
    pMetadata[1] = {OCTAVE, 96};
    pMetadata[2] = {NONE, 0};
    pMetadata[3] = {NONE, 0};
    pMetadata[4] = {NONE, 0};
    pMetadata[5] = {NONE, 0};
}

void SimpleWave::fill(CRGB *leds, long numLEDs, long t, long dt, State &state)
{
     t *= state.octave(paramIndex, 1);

    for (int i = 0; i < numLEDs; i++)
    {
        uint8_t val = cubicwave8(-t / 2 + i * 4);
        // val = dim8_video(val);
        leds[i] = CHSV(state.globalParams[0], 255 - state.patternParams[paramIndex][1], val);
    }
}