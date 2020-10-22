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
    switch (control)
    {
    case 13: // first pot on nanoKontrol
    case 74: // first encoder on beatStep
        state.globalParams[0] = 2 * value;
        break;
    case 14: // second pot on nanoKontrol
    case 71: // second encoder on Beatstep
        state.globalParams[4] = 2 * value;
        break;
    case 15: // third pot on nanoKontrol
    case 76: // third encoder on Beatstep
        state.globalParams[5] = 2 * value;
        break;
    case 2: // first fader on nanoKontrol
        state.globalParams[1] = 2 * value;
        break;
    case 3: // second fader on nanoKontrol
        state.globalParams[2] = 2 * value;
        break;
    // stop
    case 63: 
        state.globalParams[3] += 128;
        break;
    // record
    case 81:
        if (value > 0) {
            writeOutput();
        }
        break;
    case 7:
        state.incSelected();
        break;
    }
}

void OnNoteOn(byte channel, byte note, byte velocity)
{
    state.globalParams[0] += 16;
    if (state.globalParams[0] > 255)
        state.globalParams[0] -= 255;
}

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

    float du = dt * state.bpm / 120.0;

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
