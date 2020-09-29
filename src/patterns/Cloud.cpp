#include "Cloud.h"

Cloud::Cloud(int idx)
{
    pos = 0;
    paramIndex = idx;
    label = "Cloud";
    pMetadata = new paramMetadata[6];
    pMetadata[0] = {NORMAL, 128};
    pMetadata[1] = {OCTAVE, 96};
    pMetadata[2] = {NORMAL, 128};
    pMetadata[3] = {NONE, 0};
    pMetadata[4] = {NONE, 0};
    pMetadata[5] = {NONE, 0};
}

void Cloud::fill(CRGB *leds, long numLEDs, long t, long dt, State &state)
{
    t *= state.visibleOctave(1);
    dt *= state.visibleOctave(1);

    float up = .50; //uprightness();

    pos += up * dt;

    CRGB defaultVariant[numLEDs];
    CRGB fullPaletteVariant[numLEDs];

    for (int i = 0; i < numLEDs; i++)
    {
        int noise = inoise8(t / 8, (1000 + i - pos / 32) * 10);
        int slope = 2;
        int value = (slope * noise) - (slope - 1) * 255;

        if (value < 0)
            value = 0;
        if (value > 255)
            value = 255;

        uint8_t hue = inoise8(t / 32, 2000 + i * 5);
        uint8_t hueIndex = state.getPaletteHue(hue);

        uint8_t sat = inoise8(t / 16, i * 5);
        sat = scale8(sat, 100);
        sat = qadd8(sat, 128);

        defaultVariant[i] = CHSV(hueIndex, sat, value);
        fullPaletteVariant[i] = ColorFromPalette(state.currentPalette, hue, value);
    }

    nblend(leds, defaultVariant, numLEDs, 255);
    nblend(leds, fullPaletteVariant, numLEDs, state.patternParams[paramIndex][2]);
}
