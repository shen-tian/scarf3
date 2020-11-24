#include "DelayFX.h"

DelayFX::DelayFX() : Pattern("Delay")
{
    pMetadata = new paramMetadata[6];
    pMetadata[0] = {NORMAL, 32};
    //strcpy(pMetadata[0].label, "AMT");
    pMetadata[1] = {NORMAL, 0};
    pMetadata[2] = {NONE, 64};
    pMetadata[3] = {NONE, 24};
    pMetadata[4] = {NONE, 128};
    pMetadata[5] = {NONE, 0};

    for (int i = 0; i < STRAND_LENGTH; i++)
    {
        buffer[i] = CRGB::Black;
    }
}

void DelayFX::fill(CRGB *leds, long numLEDs, long t, long dt, State &state)
{
    uint8_t fadeAmount = patternParam(state, 0);
    fadeToBlackBy(buffer, STRAND_LENGTH, fadeAmount);
    fadeToBlackBy(leds, STRAND_LENGTH, patternParam(state, 1));

    // fadeToBlackBy(leds, STRAND_LENGTH, 224);
    for (int i = 0; i < numLEDs; i++){
        leds[i] = leds[i] + buffer[i];
    }
    // nblend(leds, buffer, STRAND_LENGTH, 127);
    

    memcpy(buffer, leds, sizeof(buffer));
}