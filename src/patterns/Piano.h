#ifndef PIANO_H
#define PIANO_H

#include "Pattern.h"
#include "State.h"

enum envelopePhase {REST, ATTACK, DECAY, SUSTAIN, RELEASE};

struct pianoKey {
    int intensity;
    int velocity;
    envelopePhase phase;
};

class Piano : public Pattern
{
public:
    Piano(int idx);
    void fill(CRGB *leds, long numLEDs, long t, long dt, State &state);

private:
    pianoKey keys[256];
};

#endif