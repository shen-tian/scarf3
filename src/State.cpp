#include "State.h"

void State::incSelected()
{
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
        params[bgMode][selectedIdx - 1]+=8;
        break;
    }
}

void State::decSelected()
{
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
        params[bgMode][selectedIdx - 1]-=8;
        break;
    }
}

void State::incSelect()
{
    selectedIdx++;
    if (selectedIdx > 6)
        selectedIdx = 0;
}