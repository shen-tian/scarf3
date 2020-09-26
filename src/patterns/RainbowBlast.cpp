#include "RainbowBlast.h"

RainbowBlast::RainbowBlast(int idx)
{
    paramIndex = idx;
    label = "Rainbow";
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
    float clock = t / 250. * state.visibleOctave(1);
    // float per_pixel_hue_jump = 600 / STRAND_LENGTH;
    float per_pixel_hue_jump = state.patternParams[paramIndex][2] / 8;
    float crawl_speed_factor = 100;

    for (int i = 0; i < STRAND_LENGTH; i++)
    {
        leds[STRAND_LENGTH - 1 - i] = CHSV(per_pixel_hue_jump * i + crawl_speed_factor * clock, 255, 255);
    }
}