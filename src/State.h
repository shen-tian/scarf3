#ifndef STATE_H
#define STATE_H

#include <stdint.h>
#include <FastLED.h>

#include "palettes.h"

enum paramType {NONE, NORMAL, CIRCULAR, OCTAVE };

struct paramMetadata{
    paramType type;
    uint8_t defaultValue;
};

struct patternMetadata{
    const char* label;
    paramMetadata *params;
};

class State
{
public:
    int bgMode = 0;

    uint8_t patternParams[5][6];

    uint8_t globalParams[6];
    /*
    0 is bg track
    1 - 6 are params
    ??
    */
    int selectedIdx = 0;

    int selectedLayer = 1;

    bool dim = false;

    int fps = 0;

    int currentBeat = 0;

    float bpm = 120.0;

    CRGBPalette16 currentPalette;

    State() {}

    bool playing = true;

    void registerPattern(int idx, const char *label, paramMetadata *params);
    void registerGlobalParams(paramMetadata *params);

    void incSelected();
    void decSelected();
    void zeroSelected();

    void cycleParam();
    void cycleLayer();

    void nextBeat();

    void recordTick(long tickMS);

    uint8_t visibleParam(int idx);
    paramType visibleParamType(int idx);

    float octave(int bgIdx, int idx);

    const char* getPatternLabel(int idx);

    void setupPalette();

    uint8_t getPaletteHue(uint8_t idx);

    void tryChangePatternParam(int bgIdx, int idx, int amount);
    void tryChangeGlobalParam(int idx, int amount);
    
    void tryChangeVisibleParam(int idx, int amount);

    void nextBG();
    void prevBG();

private:
    patternMetadata pMeta[5];
    patternMetadata gMeta = patternMetadata();
};

#endif