#include "Sparkle.h"

Sparkle::Sparkle() : Pattern("Sparkle")
{
    pMetadata = new paramMetadata[6];
    pMetadata[0] = {NONE, 0};
    pMetadata[1] = {NONE, 0};
    pMetadata[2] = {NONE, 0};
    pMetadata[3] = {NONE, 0};
    pMetadata[4] = {NONE, 0};
    pMetadata[5] = {NONE, 0};
}

void Sparkle::fill(CRGB *leds, long numLEDs, long t, long dt, State &state)
{
    int spot = random16(5000);
    for (int i = 0; i < 255; i++){
        if (state.notes[i] > 0)
            spot = random16(numLEDs);
    }
    if (spot < numLEDs)
        leds[spot] = CHSV(0, 0, 255);

    fadeUsingColor(leds, numLEDs, CHSV(state.globalParams[0] - 32, 5, 255 - dt / 10));
}