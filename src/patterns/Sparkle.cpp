#include "Sparkle.h"

Sparkle::Sparkle() : Pattern("Sparkle")
{
    pMetadata = new paramMetadata[6];
    pMetadata[0] = {NORMAL, 128};
    pMetadata[1] = {OCTAVE, 96};
    pMetadata[2] = {NONE, 0};
    pMetadata[3] = {NONE, 0};
    pMetadata[4] = {NONE, 0};
    pMetadata[5] = {NONE, 0};
}

void Sparkle::fill(CRGB *leds, long numLEDs, long t, long dt, State &state)
{
    int spot = random16(20 * (255 - state.globalParams[1]));
    if (spot < numLEDs)
        leds[spot] = CHSV(0, 0, 255);

    fadeUsingColor(leds, numLEDs, CHSV(state.globalParams[0] - 32, 5, 255 - dt / 10));
}