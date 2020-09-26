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

#define BRIGHTNESS 255

/**
   Whether the pattern is reversed.
*/
#define REVERSED

CRGB leds[STRAND_LENGTH];

CRGB layer0[STRAND_LENGTH];
CRGB layer1[STRAND_LENGTH];

Bounce debouncer = Bounce();
Bounce debouncer2 = Bounce();
Bounce debouncer3 = Bounce();

Encoder knobLeft(3, 4);

USBHost myusb;
MIDIDevice midi1(myusb);

State state = State();

Pattern **patterns = new Pattern *[5];

void OnControlChange(byte channel, byte control, byte value)
{
    switch (control)
    {
    case 13:
        state.globalParams[0] = 2 * value;
        break;
    case 2:
        state.globalParams[1] = 2 * value;
        break;
    case 3:
        state.globalParams[2] = 2 * value;
        break;
    case 63:
        state.globalParams[3] += 128;
    }
}

void setup()
{
    patterns[0] = new Cloud(0);
    patterns[1] = new Scarf(1);
    patterns[2] = new VariablePulse(2);
    patterns[3] = new SimpleWave(3);
    patterns[4] = new RainbowBlast(4);

    for (int i = 0; i < 5; i++)
    {
        state.registerPattern(i, patterns[i]->getLabel(), patterns[i]->getParamMetaData());
    }

    Serial.begin(9600);
    Serial.println("Scarf OS 3.0");

    initDisplay();

#if defined(BUTTON_PIN)
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(2, INPUT_PULLUP);

    debouncer.attach(1);
    debouncer.interval(20);

    debouncer2.attach(2);
    debouncer2.interval(20);
#endif

    debouncer3.attach(0);
    debouncer3.interval(20);

    myusb.begin();

    midi1.setHandleControlChange(OnControlChange);

    pinMode(7, OUTPUT);

    FastLED.addLeds<APA102, 11, 13, BGR>(leds, STRAND_LENGTH).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<1, WS2811, 10, RGB>(leds, STRAND_LENGTH).setCorrection(TypicalLEDStrip);

    FastLED.setBrightness(BRIGHTNESS);
}

void sparkle(long t, long dt)
{
    int spot = random16(20 * (255 - state.globalParams[1]));
    if (spot < STRAND_LENGTH)
        layer1[spot] = CHSV(0, 0, 255);

    fadeUsingColor(layer1, STRAND_LENGTH, CHSV(state.globalParams[0] - 32, 5, 255 - dt / 10));
}

/**
 * Test pattern for waterfall type patter
 * @param t current tick
 * @param dt ticks since last call
 * @param state overall system state
 */
void patternWaterall(long t, long dt, State &state)
{
    for (int i = STRAND_LENGTH - 1; i >= 1; i--)
    {
        layer0[i] = layer0[i - 1];
    }

    if (state.patternParams[0][0] == 0)
    {

        double v = inoise16(t * 65536 / 128) / 65535.0;

        v = pow(v, 1 + (state.patternParams[0][1] / 32));
        uint8_t value = v * 255;

        if (value < state.patternParams[0][2])
            value = 0;

        // uint8_t sat = inoise8(t + 10000);
        uint8_t sat = 255;

        layer0[0] = CHSV(state.globalParams[0], sat, value);
    }
    else
    {
        uint8_t value = ease8InOutCubic(max(255 - (t % 500), 0));
        layer0[0] = CHSV(state.globalParams[0], 255, value);
    }
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

CRGB obuff[STRAND_LENGTH];

u_int8_t fxTargetLevel = 0;
u_int8_t fxCurrentLevel = 0;

int knobPos = 0;

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

    // post process

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

    switch (state.bgMode)
    {
    case 0 ... 4:
        //patternWaterall(tick, dTick, state);
        patterns[state.bgMode]->fill(layer0, STRAND_LENGTH, tick, dTick, state);
        break;
    }

    sparkle(tick, dTick);

    memcpy(leds, layer0, sizeof(leds));

    nblend(leds, layer1, STRAND_LENGTH, state.globalParams[2]);

    if (state.dim)
    {
        fxTargetLevel = 255;
    }
    else
    {
        fxTargetLevel = 0;
    }

    if (fxTargetLevel > fxCurrentLevel)
        fxCurrentLevel = min(fxCurrentLevel + 4, fxTargetLevel);

    if (fxTargetLevel < fxCurrentLevel)
        fxCurrentLevel = max(fxCurrentLevel - 4, fxTargetLevel);

    for (int i = 0; i < STRAND_LENGTH; i++)
    {
        CHSV tempHSV = rgb2hsv_approximate(leds[i]);
        obuff[i] = CHSV(45, tempHSV.sat, tempHSV.val);
    }

    nblend(leds, obuff, STRAND_LENGTH, fxCurrentLevel);

    updateDisplay(state);

    FastLED.show(); // display this frame
    if (state.globalParams[5] > 0)
        FastLED.delay(state.globalParams[5]);
}
