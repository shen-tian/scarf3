#include "Cloud.h"

Cloud::Cloud(int idx){
    pos = 0;
    paramIndex = idx;
    label = "Cloud";
    pMetadata = new paramMetadata[6];
    pMetadata[0] = {NORMAL, 128};
    pMetadata[1] = {OCTAVE, 96};
    pMetadata[2] = {NONE, 0};
    pMetadata[3] = {NONE, 0};
    pMetadata[4] = {NONE, 0};
    pMetadata[5] = {NONE, 0};
}

void Cloud::fill(CRGB *leds, long numLEDs, long t, long dt, State &state)
{
    t *= state.visibleOctave(1);
    dt *= state.visibleOctave(1);

    float up = .5; //uprightness();

    pos += up * dt;

    for (int i = 0; i < numLEDs; i++)
    {
        uint8_t value = inoise8(t / 8, (1000 + i - pos / 32) * 10);
        value = qsub8(value, 64);
        if (value < 32)
            value = 0;
        //value = ease8InOutApprox(value);

        uint8_t hue = inoise8(t / 32, 2000 + i * 5);
        // hue = map8(hue, 0, 100);
        // hue += state.globalParams[0] - 50;

        hue = state.getPaletteHue(hue);

        uint8_t sat = inoise8(t / 16, i * 5);
        sat = scale8(sat, 100);
        sat = qadd8(sat, 128);

        leds[i] = CHSV(hue, sat, value + 32);
    }
}
