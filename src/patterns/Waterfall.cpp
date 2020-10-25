#include "Waterfall.h"

Waterfall::Waterfall() : Pattern("Sparkle")
{
    pMetadata = new paramMetadata[6];
    pMetadata[0] = {NORMAL, 128};
    pMetadata[1] = {OCTAVE, 96};
    pMetadata[2] = {NONE, 0};
    pMetadata[3] = {NONE, 0};
    pMetadata[4] = {NONE, 0};
    pMetadata[5] = {NONE, 0};
}

/**
 * Test pattern for waterfall type patter
 * @param t current tick
 * @param dt ticks since last call
 * @param state overall system state
 */
void Waterfall::fill(CRGB *leds, long numLEDs, long t, long dt, State &state)
{
    for (int i = numLEDs - 1; i >= 1; i--)
    {
        leds[i] = leds[i - 1];
    }

    if (patternParam(state, 0) == 0)
    {

        double v = inoise16(t * 65536 / 128) / 65535.0;

        v = pow(v, 1 + (patternParam(state, 0) / 32));
        uint8_t value = v * 255;

        if (value < patternParam(state, 2))
            value = 0;

        // uint8_t sat = inoise8(t + 10000);
        uint8_t sat = 255;

        leds[0] = CHSV(state.globalParams[0], sat, value);
    }
    else
    {
        uint8_t value = ease8InOutCubic(max(255 - (t % 500), 0));
        leds[0] = CHSV(state.globalParams[0], 255, value);
    }
}