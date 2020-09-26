#include "State.h"

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
            globalParams[selectedIdx - 1] += 8;
            break;
        }
        break;
    case 1:
        switch (selectedIdx)
        {
        case 0:
            bgMode++;
            if (bgMode > 4)
            {
                bgMode = 0;
            }
            break;
        default:
            patternParams[bgMode][selectedIdx - 1] += 8;
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
            globalParams[selectedIdx - 1] -= 8;
            break;
        }
        break;
    case 1:
        switch (selectedIdx)
        {
        case 0:
            bgMode--;
            if (bgMode < 0)
            {
                bgMode = 4;
            }
            break;
        default:
            patternParams[bgMode][selectedIdx - 1] -= 8;
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
            globalParams[selectedIdx - 1] = 0;
            break;
        }
        break;
    case 1:
        switch (selectedIdx)
        {
        case 0:
            bgMode = 0;
            break;
        default:
            patternParams[bgMode][selectedIdx - 1] = 0;
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
    case 1:
        return patternParams[bgMode][idx];
    default:
        return 7;
    }
}

void State::recordTick(long tickMS){
    fps = 1000.0 / tickMS;
    // fps = tickMS;
}

void State::nextBeat(){
    currentBeat++;
    if (currentBeat > 3) {
        currentBeat = 0;
    }
}

void State::registerPattern(int idx, const char *label){
    patternLabels[idx] = label;
}

const char* State::getPatternLabel(int idx){
    return patternLabels[idx];
}