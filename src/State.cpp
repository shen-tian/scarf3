#include "State.h"
#include "palettes.h"

void State::incSelected()
{
    switch (selectedLayer)
    {
    case 0:
        switch (selectedIdx)
        {
        case 0:
            bpm += 0.5;
            break;
        default:
            tryChangeGlobalParam(selectedIdx - 1, 8);
            break;
        }
        break;
    case 1 ... 2:
        switch (selectedIdx)
        {
        case 0:
            nextPattern(selectedLayer - 1);
            break;
        default:
            tryChangePatternParam(activePatternIndex(), selectedIdx - 1, 8);
            break;
        }
        break;
    }
}

void State::decSelected()
{
    switch (selectedLayer)
    {
    case 0:
        switch (selectedIdx)
        {
        case 0:
            bpm -= 0.5;
            break;
        default:
            tryChangeGlobalParam(selectedIdx - 1, -8);
            break;
        }
        break;
    case 1 ... 2:
        switch (selectedIdx)
        {
        case 0:
            prevPattern(selectedLayer - 1);
            break;
        default:
            tryChangePatternParam(activePatternIndex(), selectedIdx - 1, -8);
        }
        break;
    }
}

void State::cycleParam()
{
    selectedIdx++;
    if (selectedIdx > 6)
        selectedIdx = 0;
}

void State::zeroSelected()
{
    switch (selectedLayer)
    {
    case 0:
        switch (selectedIdx)
        {
        case 0:
            bpm = 120.0;
            break;
        default:
            globalParams[selectedIdx - 1] = gMeta.params[selectedIdx - 1].defaultValue;
            break;
        }
        break;
    case 1 ... 2:
        switch (selectedIdx)
        {
        case 0:
            selectedPattern[selectedLayer - 1] = 0;
            break;
        default:
            patternParams[activePatternIndex()][selectedIdx - 1] = pMeta[activePatternIndex()].params[selectedIdx - 1].defaultValue;
        }
        break;
    }
}

void State::cycleLayer()
{
    selectedLayer++;
    if (selectedLayer > 3)
    {
        selectedLayer = 0;
    }
}

uint8_t State::visibleParam(int idx)
{
    switch (selectedLayer)
    {
    case 0:
        return globalParams[idx];
    case 1 ... 2:
        return patternParams[activePatternIndex()][idx];
    default:
        return 7;
    }
}

paramType State::visibleParamType(int idx)
{
    switch (selectedLayer)
    {
    case 0:
        return gMeta.params[idx].type;
    case 1 ... 2:
        return pMeta[activePatternIndex()].params[idx].type;
    default:
        return NONE;
    }
}

void State::recordTick(long tickMS)
{
    fps = 1000.0 / tickMS;
    // fps = tickMS;
}

void State::nextBeat()
{
    currentBeat++;
    if (currentBeat > 3)
    {
        currentBeat = 0;
    }
}

int State::registerPattern(int layerIdx, const char *label, paramMetadata *params)
{
    int overallIndex = numPatterns;
    int curLayerIndex = patternCount[layerIdx];

    patternIndex[layerIdx][curLayerIndex] = overallIndex;

    numPatterns++;
    patternCount[layerIdx]++;

    pMeta[overallIndex].label = label;
    pMeta[overallIndex].params = params;

    for (int i = 0; i < 6; i++)
    {
        if (params[i].type != NONE)
        {
            patternParams[overallIndex][i] = params[i].defaultValue;
        }
    }

    Serial.printf("layer %i pos %i idx %i %s\n", layerIdx, curLayerIndex, overallIndex, label);

    return overallIndex;
}

int State::activePatternIndex(int layer)
{
    switch(layer){
        case 0 ... 1: 
            return patternIndex[layer][selectedPattern[layer]];
        default:
            return 0;
    }
}

int State::activePatternIndex()
{
    if (selectedLayer == 1 || selectedLayer == 2)
        return activePatternIndex(selectedLayer - 1);
    else
        return -1;
    
}

void State::registerGlobalParams(paramMetadata *params){
    //gMeta = new patternMetadata();
    gMeta.params = params;
    for (int i = 0; i < 6; i ++)
    {
        if (params[i].type != NONE){
            globalParams[i] = params[i].defaultValue;
        }
    }
}

const char *State::getPatternLabel(int idx)
{
    return pMeta[idx].label;
}

void State::setupPalette()
{
    uint8_t hueCenter = globalParams[0];
    uint8_t hueSpan = globalParams[4] / 2;

    CHSV colorStart = CHSV( hueCenter - hueSpan, 255, 255);
    CHSV colorEnd  = CHSV( hueCenter + hueSpan, 255, 255);

    currentPalette = CHSVPalette16(colorStart, colorEnd, colorStart);

    if (hueSpan > 63){
        fill_gradient(currentPalette.entries, 16, colorStart, colorEnd, colorStart, LONGEST_HUES);
    }

    switch (globalParams[5]){
        case 32 ... 63:
            currentPalette = PartyColors_p;
            break;
        case 64 ... 95:
            currentPalette = ForestColors_p;
            break;
        case 96 ... 127:
            currentPalette = RainbowColors_p;
            break;
        case 128 ... 159:
            currentPalette = aurora_gpp;
            break;
        case 160 ... 191:
            currentPalette = bhw1_06_gp;
            break;
    }
}

uint8_t State::getPaletteHue(uint8_t idx){
    CHSV color = rgb2hsv_approximate(ColorFromPalette(currentPalette, idx));
    return color.hue;
}

void State::tryChangePatternParam(int bgIdx, int idx, int amount){
    int curValue = patternParams[bgIdx][idx];

    switch(pMeta[bgIdx].params[idx].type){
    case CIRCULAR:
        patternParams[bgIdx][idx] += amount;
        break;
    case NORMAL:
        patternParams[bgIdx][idx] = constrain(curValue + amount, 0, 255);
        break;
    case OCTAVE:
        if (amount > 0){
            patternParams[bgIdx][idx] = constrain(curValue + 32, 0, 224);
        } else {
            patternParams[bgIdx][idx] = constrain(curValue - 32, 0, 224);
        }
        break;
    case NONE:
        break;
    }
}

void State::tryChangeGlobalParam(int idx, int amount){
     int curValue = globalParams[idx];

    switch(gMeta.params[idx].type){
    case CIRCULAR:
        globalParams[idx] += amount;
        break;
    case NORMAL:
        globalParams[idx] = constrain(curValue + amount, 0, 255);
        break;
    case OCTAVE:
        if (amount > 0){
            globalParams[idx] = constrain(curValue + 32, 0, 224);
        } else {
            globalParams[idx] = constrain(curValue - 32, 0, 224);
        }
        break;
    case NONE:
        break;
    }
}

void State::tryChangeVisibleParam(int idx, int amount){
    switch (selectedLayer)
    {
    case 0:    
        tryChangeGlobalParam(idx, amount);
        break;  
    case 1 ... 2: 
        tryChangePatternParam(activePatternIndex(), idx, amount);
        break;
    }
}

void State::nextPattern(int layerIdx)
{
    selectedPattern[layerIdx] = constrain(selectedPattern[layerIdx] + 1, 0, patternCount[layerIdx] - 1);
}

void State::prevPattern(int layerIdx)
{
    selectedPattern[layerIdx] = constrain(selectedPattern[layerIdx] - 1, 0, patternCount[layerIdx] - 1);
}