#include "RainbowBlast.h"

RainbowBlast::RainbowBlast() : Pattern("Rainbow")
{
    pMetadata = new paramMetadata[6];
    pMetadata[0] = {NORMAL, 128};
    pMetadata[1] = {OCTAVE, 96};
    pMetadata[2] = {NORMAL, 32};
    pMetadata[3] = {NONE, 0};
    pMetadata[4] = {NONE, 0};
    pMetadata[5] = {NONE, 0};
}

void RainbowBlast::fill(CRGB *leds, long numLEDs, long t, long dt, State &state)
{
    float clock = t / 250. * octave(state, 1);
    // float per_pixel_hue_jump = 600 / STRAND_LENGTH;
    float per_pixel_hue_jump = patternParam(state, 2) / 8;
    float crawl_speed_factor = 100;

    for (int i = 0; i < numLEDs; i++)
    {
        uint8_t hueIndex = per_pixel_hue_jump * i + crawl_speed_factor * clock;
        leds[numLEDs - 1 - i] = ColorFromPalette(state.currentPalette, hueIndex, patternParam(state, 0));
    }
}