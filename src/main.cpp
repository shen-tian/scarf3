#include <FastLED.h>
#include <Bounce2.h>
#include <Encoder.h>

#include <USBHost_t36.h>

#include "State.h"
#include "display.h"
#include "palettes.h"

#include "hal/default.h"

#include "patterns/Pattern.h"
#include "patterns/Cloud.h"
#include "patterns/Scarf.h"
#include "patterns/VariablePulse.h"
#include "patterns/SimpleWave.h"
#include "patterns/RainbowBlast.h"
#include "patterns/Sparkle.h"
#include "patterns/TestPattern.h"

CRGB leds[STRAND_LENGTH];

CRGB layer0[STRAND_LENGTH];
CRGB layer1[STRAND_LENGTH];

Bounce debouncer = Bounce();
Bounce debouncer2 = Bounce();
Bounce debouncer3 = Bounce();

Encoder knobLeft(3, 4);
int knobPos = 0;

USBHost myusb;
MIDIDevice midi1(myusb);

State state = State();

Pattern **patterns = new Pattern *[5];
Pattern **overlays = new Pattern *[1];

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
    case 10: // first endcoder on beatstep
        state.tryChangeGlobalParam(0, (value - 64)); // relative mode #1
        break;
    case 74: // second encoder on beatStep
        state.tryChangeGlobalParam(4, (value - 64)); // relative mode #1
        break;
    case 71: // third encoder on Beatstep
        state.tryChangeGlobalParam(5, (value - 64)); // relative mode #1
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
            state.bgMode = (state.bgMode + 5 - 1) % 5;
        break;
    case 61: // next track on nanoKontrol
        if (value == 127)
            state.bgMode = (state.bgMode + 1) % 5;
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

void OnNoteOn(byte channel, byte note, byte velocity)
{
    state.globalParams[0] += 16;
    if (state.globalParams[0] > 255)
        state.globalParams[0] -= 255;
}

paramMetadata globalParamsMeta[6];

void setup()
{
    patterns[0] = new TestPattern(0);
    patterns[1] = new Cloud(1);
    patterns[2] = new VariablePulse(2);
    patterns[3] = new SimpleWave(3);
    patterns[4] = new RainbowBlast(4);

    for (int i = 0; i < 5; i++)
    {
        state.registerPattern(i, patterns[i]->getLabel(), patterns[i]->getParamMetaData());
    }

    globalParamsMeta[0] = {CIRCULAR, 128};
    globalParamsMeta[1] = {NORMAL, 0};
    globalParamsMeta[2] = {NONE, 0};
    globalParamsMeta[3] = {OCTAVE, 0};
    globalParamsMeta[4] = {NORMAL, 0};
    globalParamsMeta[5] = {OCTAVE, 0};

    state.registerGlobalParams(globalParamsMeta);

    overlays[0] = new Sparkle(0);

    Serial.begin(9600);
    Serial.println("Scarf OS 3.0");

    initDisplay();

    pinMode(1, INPUT_PULLUP);
    pinMode(2, INPUT_PULLUP);

    debouncer.attach(1);
    debouncer.interval(20);

    debouncer2.attach(2);
    debouncer2.interval(20);

    debouncer3.attach(0);
    debouncer3.interval(20);

    myusb.begin();

    midi1.setHandleControlChange(OnControlChange);
    midi1.setHandleNoteOn(OnNoteOn);

    FastLED.addLeds<APA102, 11, 13, BGR>(leds, STRAND_LENGTH);
    FastLED.addLeds<1, WS2811, 10, RGB>(leds, STRAND_LENGTH);

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
    dTick = nowV - v;
    v = nowV;
    t = nowT;
}

void loop()
{
    updateTransport();

    myusb.Task();

    midi1.read(1);

    debouncer.update();
    debouncer2.update();
    debouncer3.update();

    if (debouncer3.fell())
    {
        state.zeroSelected();
    }

    if (debouncer2.fell())
    {
        state.cycleParam();
    }

    if (debouncer.fell())
    {
        state.cycleLayer();
    }

    int newKnobPos = knobLeft.read();

    if (newKnobPos - knobPos >= 4)
    {
        state.incSelected();
        knobPos = newKnobPos;
    }

    if (knobPos - newKnobPos >= 4)
    {
        state.decSelected();
        knobPos = newKnobPos;
    }

    state.setupPalette();

    patterns[state.bgMode]->fill(layer0, STRAND_LENGTH, tick, dTick, state);
    overlays[0]->fill(layer1, STRAND_LENGTH, tick, dTick, state);

    memcpy(leds, layer0, sizeof(leds));
    nblend(leds, layer1, STRAND_LENGTH, state.globalParams[2]);

    updateDisplay(state);

    FastLED.show(); // display this frame
    // FastLED.delay(0);
}
