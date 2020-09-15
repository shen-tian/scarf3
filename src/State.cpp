#include "State.h"

void State::incMode()
{
    if (selectedIdx == 0)
    {
        bgMode++;
        if (bgMode > 4)
        {
            bgMode = 0;
        }
    }
    else if (selectedIdx == 1)
    {
        fooParam++;
    }
}

void State::decMode()
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
    case 1:
        fooParam--;
        break;
    }
}

void State::incSelect()
{
    selectedIdx++;
    if (selectedIdx > 1)
        selectedIdx = 0;
}