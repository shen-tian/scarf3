#include "Scarf.h"

/**
 * Get a byte that cycles from 0-255, at a specified rate
 * typically, assign mil using mills();
 * rates, approximately (assuming 256ms in a second :P)
 * 8: 4hz
 * 7: 2hz
 * 6: 1hz
 * 5: 1/2hz
 * 4: 1/4hz
 * 3: 1/8hz
 * 2: 1/16hz
 * 1: 1/32hz
 */

byte Scarf::getClock(unsigned long mil, byte rate)
{
    return mil >> (8 - rate) % 256;
}

byte Scarf::modDist(byte x, byte y)
{
    return min(min(abs(x - y), abs(x - y + 256)), abs(x - y - 256));
}

Scarf::Scarf(int idx) : Pattern(idx, "Scarf")
{
    pos = 0;

    pMetadata = new paramMetadata[6];
    pMetadata[0] = {NORMAL, 128};
    pMetadata[1] = {OCTAVE, 128};
    pMetadata[2] = {NONE, 0};
    pMetadata[3] = {NONE, 0};
    pMetadata[4] = {NONE, 0};
    pMetadata[5] = {NONE, 0};
}

void Scarf::fill(CRGB *leds, long numLEDs, long t, long dt, State &state)
{
    t *= octave(state, 1);
    dt *= octave(state, 1);

    byte color = getClock(t, 2);
    byte pulse = inoise8(t / 4.) * .5;
    byte drift = getClock(t, 3);
    pulse += drift;

    // pulse = getClock(t, 4) + 4 * sin((t * PI)/255.0);

    if (pulse > 255)
        pulse -= 255;

    for (byte pix = 0; pix < numLEDs; pix++)
    {
        // location of the pixel on a 0-RENDER_RANGE scale.
        byte dist = pix * 255 / numLEDs;

        byte delta = modDist(dist, pulse);
        // linear ramp up of brightness, for those within 1/8th of the reference point
        float value = max(255 - 6 * delta, 64);

        // hue selection. Mainly driving by c, but with some small shifting along
        // the length of the strand.

        // sweep of a subset of the spectrum.
        float x = color / 255. + pix * 128. / numLEDs;
        if (x >= 1)
            x -= 1.;

        float hue = 255 * HUE_START + cubicwave8(x) * (HUE_END - HUE_START);

        hue += state.globalParams[0] - 175;

        byte loc = pix;
#if defined(REVERSED)
        loc = STRAND_LENGTH - 1 - pix;
#endif
        uint8_t hueIndex = value / 2;

        leds[loc] = CHSV(hue, 255 * SATURATION, value);
        leds[loc] = CHSV(state.getPaletteHue(hueIndex), 255 * SATURATION, value);
    }
}