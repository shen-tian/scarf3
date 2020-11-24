#include "ColourFX.h"

ColourFX::ColourFX() : Pattern("Colour")
{
    pMetadata = new paramMetadata[6];
    pMetadata[0] = {CIRCULAR, 128};
    strcpy(pMetadata[0].label, "HUE");
    pMetadata[1] = {NORMAL, 128};
    strcpy(pMetadata[1].label, "SAT");
    pMetadata[2] = {NORMAL, 128};
    strcpy(pMetadata[2].label, "VAL");
    pMetadata[3] = {NONE, 24};
    pMetadata[4] = {NONE, 128};
    pMetadata[5] = {NONE, 0};
}

void ColourFX::fill(CRGB *leds, long numLEDs, long t, long dt, State &state)
{
    int hueAdjust = patternParam(state, 0) - 128;
    int satAdjust = patternParam(state, 1) - 128;
    int valAdjust = patternParam(state, 2) - 128;

    for (int i = 0; i < numLEDs; i++){
        if (hueAdjust != 0 || satAdjust != 0 || valAdjust != 0){
            CHSV pixel = rgb2hsv_approximate(leds[i]);
         
            pixel.hue += hueAdjust;
            pixel.sat = constrain(pixel.sat + satAdjust, 0, 255);
            pixel.value = constrain(pixel.val + valAdjust, 0, 255);

            leds[i] = pixel;
        }
    }
}