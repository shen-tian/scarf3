#include <FastLED.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Bounce2.h>
#include <Encoder.h>

#include <USBHost_t36.h>

#include "State.h"
#include "display.h"

#include "hal/default.h"

#define BRIGHTNESS 255

/**
   Whether the pattern is reversed.
*/
#define REVERSED

Adafruit_BNO055 bno = Adafruit_BNO055(-1, 0x29);
boolean imuPresent = true;

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
    }
}

void setup()
{
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

    if (!bno.begin())
    {
        Serial.println("no BNO055 detected");
        imuPresent = false;
    }
}

// Get a byte that cycles from 0-255, at a specified rate
// typically, assign mil using mills();
// rates, approximately (assuming 256ms in a second :P)
// 8: 4hz
// 7: 2hz
// 6: 1hz
// 5: 1/2hz
// 4: 1/4hz
// 3: 1/8hz
// 2: 1/16hz
// 1: 1/32hz
byte getClock(unsigned long mil, byte rate)
{
    return mil >> (8 - rate) % 256;
}

// messy, but some sort of least-of-3 distances, allowing wraping.
byte modDist(byte x, byte y)
{
    return min(min(abs(x - y), abs(x - y + 256)), abs(x - y - 256));
}

// partymode ala nazgul
void pattern_rainbow_blast(long t)
{
    float clock = t / 1000.;
    // float per_pixel_hue_jump = 600 / STRAND_LENGTH;
    float per_pixel_hue_jump = state.patternParams[3][0] / 8;
    float crawl_speed_factor = 100;

    for (int i = 0; i < STRAND_LENGTH; i++)
    {
        layer0[STRAND_LENGTH - 1 - i] = CHSV(per_pixel_hue_jump * i + crawl_speed_factor * clock, 255, 255);
    }
}

/**
    Pattern definition. The program cycles through a range on the wheel, and
    back again. This defines the boundaries. Note that wraparound for the full
    rainbow is not enabled. Would take special case code.
*/

#define HUE_START .5
#define HUE_END .75
#define SATURATION .8

void pattern_classic(long t, long dt)
{
    byte color = getClock(t, 2);
    byte pulse = inoise8(t / 4.) * .5;
    byte drift = getClock(t, 3);
    pulse += drift;

    // pulse = getClock(t, 4) + 4 * sin((t * PI)/255.0);

    if (pulse > 255)
        pulse -= 255;

    for (byte pix = 0; pix < STRAND_LENGTH; pix++)
    {
        // location of the pixel on a 0-RENDER_RANGE scale.
        byte dist = pix * 255 / STRAND_LENGTH;

        byte delta = modDist(dist, pulse);
        // linear ramp up of brightness, for those within 1/8th of the reference point
        float value = max(255 - 6 * delta, 64);

        // hue selection. Mainly driving by c, but with some small shifting along
        // the length of the strand.

        // sweep of a subset of the spectrum.
        float x = color / 255. + pix * 128. / STRAND_LENGTH;
        if (x >= 1)
            x -= 1.;

        float hue = 255 * HUE_START + cubicwave8(x) * (HUE_END - HUE_START);

        hue += state.globalParams[0] - 175;

        byte loc = pix;
#if defined(REVERSED)
        loc = STRAND_LENGTH - 1 - pix;
#endif

        layer0[loc] = CHSV(hue, 255 * SATURATION, value);
    }
}

void pattern_warm_white(long t)
{
    fill_solid(leds, STRAND_LENGTH, CRGB(255, 147, 41));
    fade_video(leds, STRAND_LENGTH, 192);
}

float u = 1;

float uprightness()
{
    if (!imuPresent)
        return .5;

    imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

    float dist = euler.z() - 90;
    if (dist < -180)
        dist += 360;

    float reading = (90 - abs(dist)) / 90;

    u = .99 * u + .01 * reading;

    return reading;
}

long pos = 0;

void patternCloud(long t, long dt)
{
    float up = uprightness();

    pos += up * dt;

    CRGB buff[STRAND_LENGTH];
    for (int i = 0; i < STRAND_LENGTH; i++)
    {
        uint8_t value = inoise8(t / 8, (1000 + i - pos / 32) * 10);
        value = qsub8(value, 64);
        if (value < 32)
            value = 0;
        //value = ease8InOutApprox(value);

        uint8_t hue = inoise8(t / 32, 2000 + i * 5);
        hue = map8(hue, 0, 100);
        hue += state.globalParams[0] - 50;

        uint8_t sat = inoise8(t / 16, i * 5);
        sat = scale8(sat, 100);
        sat = qadd8(sat, 128);

        if (i % 1 == 0)
            buff[i] = CHSV(hue, sat, value + 32);
        else
            buff[i] = CRGB::Black;
    }
    nblend(layer0, buff, STRAND_LENGTH, 255);

    //fade_video(layer0, STRAND_LENGTH, 128);
}

void simpleWave(long t, long dt, State &state)
{
    for (int i = 0; i < STRAND_LENGTH; i++)
    {
        uint8_t val = cubicwave8(-t / 16 + i * 4);
        // val = dim8_video(val);
        layer0[i] = CHSV(state.globalParams[0], 255 - state.patternParams[2][1], val);
    }
}

// return a cyclical (sine wave) value between min and max
float xcycle(float t, float period, float min, float max)
{
    return .5 * (min + max) - .5 * (max - min) * cos(t / period * (2 * PI));
}

// blend linearly between a and b, fraction=0 returns a, fraction=1 returns b
float xblend(float a, float b, float fraction)
{
    return (1 - fraction) * a + fraction * b;
}

int brightness_to_value(float brightness, float min_brightness)
{
    return xblend(min_brightness, 1., brightness) * 128;
}

void pattern_variable_pulses(long t, State &state)
{
    float clock = t / 1000.;
    // int BASE_HUE = 175;
    float density_scale_factor = STRAND_LENGTH / 36.;

    float period = 30; // s
    float peakedness = 3;
    float min_pulse_width = 5. * density_scale_factor;
    float max_pulse_width = STRAND_LENGTH * 2.5;
    float crawl_speed_factor = 1; // around 1 is the sweet spot; changing this too much seems to look much worse
    float min_brightness = .05;

    // cycle in the inverse space to balance short vs. long pulses better
    float pulse_width = 1. / xcycle(clock, period, 1. / min_pulse_width, 1. / max_pulse_width);
    float crawl_offset = crawl_speed_factor * clock;
    for (int i = 0; i < STRAND_LENGTH; i++)
    {
        float brightness = xcycle(STRAND_LENGTH - i + crawl_offset * pulse_width, pulse_width, 0, 1);
        brightness = pow(brightness, peakedness);
        int value = brightness_to_value(brightness, min_brightness);
        layer0[i] = CHSV(state.globalParams[0], 255, value);
    }
}

void sparkle(long t)
{
    int spot = random16(20 * (255 - state.globalParams[1]));
    if (spot < STRAND_LENGTH)
        layer1[spot] = CHSV(0, 0, 255);

    fadeUsingColor(layer1, STRAND_LENGTH, CRGB(230, 239, 245));

    // leds[t - last_t] = CRGB(255,0,0);
    // last_t = t;
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

unsigned long last_t = 0;
unsigned long lastBeat = 0;

unsigned long tick = 0;

CRGB obuff[STRAND_LENGTH];

u_int8_t fxTargetLevel = 0;
u_int8_t fxCurrentLevel = 0;

int knobPos = 0;

void loop()
{
    unsigned long t = millis();

    state.recordTick(t - last_t);

    int msPerBeat = 60 * 1000.0 / state.bpm;

    // TODO: capture the frac part to limit drift.
    int dTick = (t - last_t) * state.bpm / 120.0;

    uint8_t multiple = state.globalParams[1];

    if (multiple > 32)
        dTick *= 2;
    if (multiple > 64)
        dTick *= 2;
    if (multiple > 96)
        dTick *= 2;
    if (multiple > 128)
        dTick *= 2;
    if (multiple > 160)
        dTick *= 2;
    if (multiple > 192)
        dTick *= 2;
    if (multiple > 224)
        dTick *= 2;

    dTick /= 4;

    tick += dTick;

    while (t - lastBeat > msPerBeat)
    {
        state.nextBeat();
        lastBeat += msPerBeat;
    }

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
    case 0:
        // patternWaterall(tick, dTick, state);
        patternCloud(tick, dTick);
        break;
    case 1:
        pattern_classic(tick, dTick);
        break;
    case 2:
        simpleWave(tick, dTick, state);
        break;
    case 3:
        pattern_rainbow_blast(tick);
        break;
    case 4:
        pattern_variable_pulses(tick, state);
        break;
    default:
        break;
    }

    sparkle(tick);

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

    last_t = t;

    updateDisplay(state);

    FastLED.show(); // display this frame
    if (state.globalParams[5] > 0)
        FastLED.delay(state.globalParams[5]);
}
