#ifndef PARTICLES_H
#define PARTICLES_H

#include "State.h"

#include "Pattern.h"
#include "../State.h"

#define MAX_PARTICLES 16

struct particle{
    float location;
    float velocity;
};

class Particles : public Pattern
{
public:
    Particles();
    void fill(CRGB *leds, long numLEDs, long t, long dt, State &state);

private:
    particle particles[MAX_PARTICLES];
};

#endif