#include <FastLED.h>
#include <USBHost_t36.h>

#include "State.h"
#include "display.h"
#include "palettes.h"
#include "PhysicalControls.h"

#include "hal/default.h"

#include "patterns/Pattern.h"
#include "patterns/Cloud.h"
#include "patterns/Scarf.h"
#include "patterns/VariablePulse.h"
#include "patterns/SimpleWave.h"
#include "patterns/RainbowBlast.h"
#include "patterns/Sparkle.h"
#include "patterns/TestPattern.h"
#include "patterns/Piano.h"
#include "patterns/Fire2012.h"
#include "patterns/Particles.h"

#include "patterns/DelayFX.h"
#include "patterns/ColourFX.h"

CRGB leds[STRAND_LENGTH];

CRGB layer0[STRAND_LENGTH];
CRGB layer1[STRAND_LENGTH];

USBHost myusb;
MIDIDevice midi1(myusb);

PhysicalControls* phyControls;

State state = State();

Pattern **patterns = new Pattern *[32];

void writeOutput(){
    for (int i = 0; i < 10; i++){
        Serial.printf("%d, %d, %d, %d\n", i, leds[i].red, leds[i].green, leds[i].blue);
    }
    Serial.println();
}

void OnControlChange(byte channel, byte control, byte value)
{
    Serial.printf("#CC %i %i\n", control, value);
    switch (control)
    {
    // visible controls on Beatstep (left set of encoders, relative mode #1)
    case 10: 
        if (value > 64)
            state.nextPattern(1);
        else
            state.prevPattern(1);
        break;
        break;
    case 74:
        state.tryChangeVisibleParam(0, (value - 64));
        break;
    case 71:
        state.tryChangeVisibleParam(1, (value - 64));
        break;
    case 76:
        state.tryChangeVisibleParam(2, (value - 64));
        break;
    case 114:
        if (value > 64)
            state.nextPattern(0);
        else
            state.prevPattern(0);
        break;
    case 18:
        state.tryChangeVisibleParam(3, (value - 64));
        break;
    case 19:
        state.tryChangeVisibleParam(4, (value - 64));
        break;
    case 16:
        state.tryChangeVisibleParam(5, (value - 64));
        break;

    // global params on Beatstep (right set of encoders, relative mode #1)


    case 77:
        // ??
        break;
    case 93:
        state.tryChangeGlobalParam(0, (value - 64));
        break;
    case 73:
        state.tryChangeGlobalParam(1, (value - 64));
        break;
    case 75:
        state.tryChangeGlobalParam(2, (value - 64));
        break;
    case 17:
        // ??
        break;
    case 91:
        state.tryChangeGlobalParam(3, (value - 64));
        break;
    case 79:
        state.tryChangeGlobalParam(4, (value - 64));
        break;
    case 72:
        state.tryChangeGlobalParam(5, (value - 64));
        break;

    case 13: // first pot on nanoKontrol
        state.globalParams[0] = 2 * value;
        break;
    case 14: // second pot on nanoKontrol
        state.globalParams[4] = 2 * value;
        break;
    case 15: // third pot on nanoKontrol
        state.globalParams[5] = 2 * value;
        break;
    case 2: // first fader on nanoKontrol
        state.globalParams[1] = 2 * value;
        break;
    case 3: // second fader on nanoKontrol
        state.globalParams[2] = 2 * value;
        break;
    case 82: // jog dial on nanoKontrol in sign magnitude mode
        if (value < 64)
            state.bpm += value * 0.5;
        else
            state.bpm -= (value - 64) * 0.5;        
        break;
    case 60: // prev track on nanoKontrol
        if (value == 127)
            state.prevPattern(0);
        break;
    case 61: // next track on nanoKontrol
        if (value == 127)
            state.nextPattern(0);
        break;
    case 63: // stop on nanoKontrol
        if (value == 127)
            state.playing = false;
        break;
    case 80: // play on nanoKontorl
        if (value == 127)
            state.playing = true;
        break;
    case 62: // back on nanoKontrol
        if (value == 127)
            state.globalParams[3] = 127;
        if (value == 0)
            state.globalParams[3] = 0;
        break;
    case 81: // record
        if (value > 0) {
            writeOutput();
        }
        break;
    }
}

void onNoteOn(byte channel, byte note, byte velocity)
{
    Serial.printf("#Note On  %03d %03d\n", note, velocity);
    state.notes[note] = velocity * 2;\
}

void onNoteOff(byte channel, byte note, byte velocity)
{
    Serial.printf("#Note Off %03d %03d\n", note, velocity);
    state.notes[note] = 0;
}

paramMetadata globalParamsMeta[6];

void setup()
{
    patterns[0] = new TestPattern();
    patterns[1] = new Cloud();
    patterns[2] = new VariablePulse();
    patterns[3] = new Particles();
    patterns[4] = new RainbowBlast();
    patterns[5] = new Fire2012();


    patterns[6] = new Piano(); 
    patterns[7] = new Sparkle();
    patterns[8] = new Fire2012();

    patterns[9] = new DelayFX();
    patterns[10] = new ColourFX();

    for (int i = 0; i <= 5; i++)
    {
        int assignedIdx = state.registerPattern(0, patterns[i]->getLabel(), patterns[i]->getParamMetaData());
        patterns[i]->setParamIdx(assignedIdx);
    }

    for (int i = 6; i <= 8; i++)
    {
        int assignedIdx = state.registerPattern(1, patterns[i]->getLabel(), patterns[i]->getParamMetaData());
        patterns[i]->setParamIdx(assignedIdx);
    }

    for (int i = 9; i <=10; i++)
    {
        int assignedIdx = state.registerPattern(2, patterns[i]->getLabel(), patterns[i]->getParamMetaData());
        patterns[i]->setParamIdx(assignedIdx);
    }

    globalParamsMeta[0] = {CIRCULAR, 170};
    strcpy(globalParamsMeta[0].label, "HUE");
    globalParamsMeta[1] = {NORMAL, 0};
    globalParamsMeta[2] = {NORMAL, 192};
    globalParamsMeta[3] = {NORMAL, 0};
    globalParamsMeta[4] = {NORMAL, 91};
    strcpy(globalParamsMeta[4].label, "WTH");
    globalParamsMeta[5] = {OCTAVE, 0};
    strcpy(globalParamsMeta[5].label, "PLT");

    state.registerGlobalParams(globalParamsMeta);

    Serial.println(state.activePatternIndex(1));

    Serial.begin(9600);
    Serial.println("Scarf OS 3.0");

    initDisplay();

    phyControls = new PhysicalControls(&state);

    delay(2000);

    myusb.begin();

    midi1.setHandleControlChange(OnControlChange);
    midi1.setHandleNoteOn(onNoteOn);
    midi1.setHandleNoteOff(onNoteOff);

    FastLED.addLeds<APA102, 11, 13, BGR>(leds, STRAND_LENGTH);
    FastLED.addLeds<1, WS2811, 10, GRB>(leds, STRAND_LENGTH);

    FastLED.setBrightness(255);
}

unsigned long tick = 0;
unsigned long dTick;

unsigned long t = 0;
unsigned long u = 3600000;
unsigned long uLastBeat = u;
unsigned long v = v;

void updateTransport()
{
    unsigned long nowT = millis();

    float dt = (nowT - t);
    // TODO: figure out how this does/doesn't drift out of sync

    float du = state.playing ? dt * state.bpm / 120.0 : 0;

    if (state.globalParams[3] > 0)
    {
        du = -du;
    }

    u += du;

    // ensure uLastBeat is before u, for purposes of reversing
    while (uLastBeat > u)
    {
        uLastBeat -= 500;
        // state.prevBeat ?
    }

    // Stutter
    float exponent = pow(0.5, state.globalParams[1] / 64);
    unsigned long nowV = uLastBeat + pow((u - uLastBeat) / 500.0, exponent) * 500;

    while (u - uLastBeat >= 500)
    {
        state.nextBeat();
        uLastBeat += 500;
    }

    // for fps cal. Not sure how NB?
    state.recordTick(nowT - t);

    tick = nowV;
    dTick = constrain(nowV - v, 0, (unsigned long) 100); // check dTick is not huge on init
    v = nowV;
    t = nowT;
}

void loop()
{
    phyControls->update();
    updateTransport();

    myusb.Task();

    midi1.read(1);

    state.setupPalette();

    // Serial.println(dTick);

    patterns[state.activePatternIndex(0)]->fill(layer0, STRAND_LENGTH, tick, dTick, state);
    patterns[state.activePatternIndex(1)]->fill(layer1, STRAND_LENGTH, tick, dTick, state);

    memcpy(leds, layer0, sizeof(leds));
    nblend(leds, layer1, STRAND_LENGTH, state.globalParams[2]);

    patterns[9]->fill(leds, STRAND_LENGTH, tick, dTick, state);
    patterns[10]->fill(leds, STRAND_LENGTH, tick, dTick, state);

    state.analyze(leds);

    updateDisplay(state);

    FastLED.show(); // display this frame
    // FastLED.delay(0);
}
