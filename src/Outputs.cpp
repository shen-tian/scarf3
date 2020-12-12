#include "Outputs.h"

Outputs::Outputs()
{
    FastLED.addLeds<APA102, 11, 13, BGR>(apa102c_0, STRAND_LENGTH);
    FastLED.addLeds<1, WS2811, 10, GRB>(ws2811_0, STRAND_LENGTH);

    FastLED.setBrightness(255);
}

void Outputs::sampleTo(CRGB *source, CRGB *target, int targetCount)
{
    for (int i = 0; i < targetCount; i++)
    {
        double left, right, frac;
        double idx = i * (double)STRAND_LENGTH / (double)targetCount;
        modf(idx, &left);
        right = left + 1;
        frac = idx - left;

        CRGB col = source[(int)left];
        blend(col, source[(int)right], 1 - frac);

        target[i] = col;
    }

    for (int i = targetCount; i < STRAND_LENGTH; i++)
    {
        target[i] = CRGB::Black;
    }
}

void Outputs::display(CRGB *leds)
{
    sampleTo(leds, apa102c_0, 144);
    sampleTo(leds, ws2811_0, 59);

    FastLED.show();
}