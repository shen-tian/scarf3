#include "Piano.h"

Piano::Piano() : Pattern("Piano")
{
    pMetadata = new paramMetadata[6];
    pMetadata[0] = {NORMAL, 255};
    pMetadata[1] = {NORMAL, 128};
    pMetadata[2] = {NORMAL, 8};
    pMetadata[3] = {NONE, 0};
    pMetadata[4] = {NONE, 0};
    pMetadata[5] = {NONE, 0};

    for (int i = 0; i < 256; i++){
        keys[i] = {0, 0, REST};
    }
}

void Piano::fill(CRGB *leds, long numLEDs, long t, long dt, State &state)
{
    for (int i = 0; i < 256; i++) {

        int attackRate = patternParam(state, 0) * 10.0 / dt;
        int decayRate = patternParam(state, 1) * 1.25 / dt;
        float sustain = 0.5;
        int releaseRate = patternParam(state, 2) * 1.25 / dt;

        switch (keys[i].phase) {
            case REST:
                if (state.notes[i] > 0){
                    keys[i].phase = ATTACK;
                    keys[i].velocity = state.notes[i];
                }
                break;
            case ATTACK:
                if (keys[i].intensity < keys[i].velocity){
                    keys[i].intensity = constrain(keys[i].intensity + attackRate, 0, keys[i].velocity);
                } else {
                    keys[i].phase = DECAY;
                }
                break;
            case DECAY:
                if (keys[i].intensity > keys[i].velocity * sustain){
                    keys[i].intensity = constrain(keys[i].intensity - decayRate, keys[i].velocity * sustain, 255);
                }
                else {
                    keys[i].phase = SUSTAIN;
                }
                break;
            case SUSTAIN:
                if (state.notes[i] == 0) {
                    keys[i].phase = RELEASE;
                }
                break;
            case RELEASE:
                if (state.notes[i] > 0){
                    keys[i].phase = ATTACK;
                    keys[i].velocity = state.notes[i];
                } else if (keys[i].intensity > 0){
                    keys[i].intensity = constrain(keys[i].intensity - releaseRate, 0, 255);
                } else {
                    keys[i].phase = REST;
                    keys[i].velocity = 0;
                }
                break; 
        }
    }

    for (int i = 0; i < numLEDs; i ++){
        int noteIdx = 36 + i / 10;
        leds[i] = CHSV(0, 0, keys[noteIdx].intensity);
    }
}