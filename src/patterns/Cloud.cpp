#include "Cloud.h"

Cloud::Cloud() : Pattern("Cloud")
{
    pos = 0;
    pMetadata = new paramMetadata[6];
    pMetadata[0] = {NORMAL, 128};
    strcpy(pMetadata[0].label, "LVL");
    pMetadata[1] = {OCTAVE, 96};
    strcpy(pMetadata[1].label, "SPD");
    pMetadata[2] = {NORMAL, 128}; // blend of palette sat v.s. full
    strcpy(pMetadata[2].label, "MIX");
    pMetadata[3] = {NORMAL, 64}; // noise octaves
    strcpy(pMetadata[3].label, "OCT");
    pMetadata[4] = {NORMAL, 128}; // slope (1 - 3);
    strcpy(pMetadata[4].label, "SLP");
    pMetadata[5] = {NONE, 0};
}

uint8_t Cloud::fractalNoise(uint16_t x, uint16_t y, int octave) {
  float r = 0;
  float amp = 1.0;
  for (int oct = 0; oct < octave; oct++) {
    r += inoise8(x, y) * amp;
    amp /= 2;
    x *= 2;
    y *= 2;
  }
  return (uint8_t) r;
}

void Cloud::fill(CRGB *leds, long numLEDs, long t, long dt, State &state)
{
    t *= octave(state, 1);
    dt *= octave(state, 1);

    float up = 0.5; //uprightness();

    pos += up * dt;

    int octaves = (patternParam(state, 3)) / 32 + 1;

    CRGB defaultVariant[numLEDs];
    CRGB fullPaletteVariant[numLEDs];

    for (int i = 0; i < numLEDs; i++)
    {
        int noise = fractalNoise(t / 8, (1000 + i - pos / 32) * 10, octaves);
        float slope = 1 + patternParam(state, 4) / 128.0;
        int value = constrain((slope * noise) - (slope - 1) * 255, 0, 255);
        //int value = noise;

        uint8_t hue = inoise8(t / 32, 2000 + i * 5);
        uint8_t hueIndex = state.getPaletteHue(hue);

        uint8_t sat = inoise8(t / 16, i * 5);
        sat = scale8(sat, 100);
        sat = qadd8(sat, 128);

        defaultVariant[i] = CHSV(hueIndex, sat, value);
        fullPaletteVariant[i] = ColorFromPalette(state.currentPalette, hue, value);
        
    }

    nblend(leds, defaultVariant, numLEDs, 255);
    nblend(leds, fullPaletteVariant, numLEDs, patternParam(state, 2));

    fadeToBlackBy(leds, numLEDs, 255 - patternParam(state, 0));
}
