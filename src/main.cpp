#include <FastLED.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Bounce2.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Encoder.h>

#ifdef __AVR__
#include <avr/power.h>
#endif

#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
  // Required for Serial on Zero based boards
  #define Serial SERIAL_PORT_USBVIRTUAL
#endif

#include "hal/default.h"

#define BRIGHTNESS 255

/**
   Whether the pattern is reversed.
*/
#define REVERSED

Adafruit_BNO055 bno = Adafruit_BNO055(-1, 0x29);
boolean imuPresent = true;

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64);

CRGB leds[STRAND_LENGTH];

CRGB layer0[STRAND_LENGTH];
CRGB layer1[STRAND_LENGTH];

Bounce debouncer = Bounce();
Bounce debouncer2 = Bounce();

Encoder knobLeft(3, 4);

void initDisplay()
{

  Serial.println("Display: init");
  // initialize with the I2C addr 0x3C (for the 128x32)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(2);

  // upside down
  display.setRotation(2);
  display.setTextColor(WHITE);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("hello.");
  display.display();
}

void setup()
{
    Serial.begin(9600);
    Serial.println("Scarf OS 3.0");

    initDisplay();

    #if defined(BUTTON_PIN)
    pinMode(BUTTON_PIN,INPUT_PULLUP);
    pinMode(2, INPUT_PULLUP);

    debouncer.attach(BUTTON_PIN);
    debouncer.interval(20);

    debouncer2.attach(2);
    debouncer2.interval(20);
    #endif

    pinMode(7, OUTPUT);

    FastLED.addLeds<APA102, 11, 13, BGR>(leds, STRAND_LENGTH).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<1, WS2811, 10, RGB>(leds, STRAND_LENGTH).setCorrection(TypicalLEDStrip);

    FastLED.setBrightness(BRIGHTNESS);

    if(!bno.begin())
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
    float per_pixel_hue_jump = 600 / STRAND_LENGTH;
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
        if (value < 32) value = 0;
        //value = ease8InOutApprox(value);

        uint8_t hue = inoise8(t / 32, 2000 + i * 5);
        hue = map8(hue, 120, 220);

        uint8_t sat = inoise8(t/ 16, i * 5);
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

void simpleWave(long t, long dt)
{
    for (int i = 0; i < STRAND_LENGTH; i++)
    {
        uint8_t val = cubicwave8(- t / 16 + i * 4);
        // val = dim8_video(val);
        layer0[i] = CHSV(192, 255, val);
    }
}

// return a cyclical (sine wave) value between min and max
float xcycle(float t, float period, float min, float max) {
  return .5*(min+max) - .5*(max-min) * cos(t / period * (2*PI));
}

// blend linearly between a and b, fraction=0 returns a, fraction=1 returns b
float xblend(float a, float b, float fraction) {
  return (1 - fraction) * a + fraction * b;
}

int brightness_to_value(float brightness, float min_brightness) {
  return xblend(min_brightness, 1., brightness) * 128;
}

void pattern_variable_pulses(long t) {
    float clock = t / 1000.;
    int BASE_HUE = 175;
    float density_scale_factor = STRAND_LENGTH / 36.;

    float period = 30; // s
    float peakedness = 3;
    float min_pulse_width = 5. * density_scale_factor;
    float max_pulse_width = STRAND_LENGTH * 2.5;
    float crawl_speed_factor = 1;  // around 1 is the sweet spot; changing this too much seems to look much worse
    float min_brightness = .05;

    // cycle in the inverse space to balance short vs. long pulses better
    float pulse_width = 1. / xcycle(clock, period, 1./min_pulse_width, 1./max_pulse_width);
    float crawl_offset = crawl_speed_factor * clock;
    for (int i = 0; i < STRAND_LENGTH; i++) {
        float brightness = xcycle(STRAND_LENGTH - i + crawl_offset*pulse_width, pulse_width, 0, 1);
        brightness = pow(brightness, peakedness);
        int value = brightness_to_value(brightness, min_brightness);
        layer0[i] = CHSV(BASE_HUE, 255, value);
    }
}

void sparkle(long t)
{
    int spot = random16(5000);
    if (spot < STRAND_LENGTH)
        layer1[spot] = CHSV(0, 0, 255);

    fadeUsingColor(layer1,STRAND_LENGTH,CRGB(230, 239, 245));

    // leds[t - last_t] = CRGB(255,0,0);
    // last_t = t;
}

int mode = 0;

void cycleModes(long t)
{
    mode++;
    if (mode > 4) {
        mode = 0;
    }
}

long last_t = 0;

bool dim = false;

CRGB obuff[STRAND_LENGTH];

u_int8_t fxTargetLevel = 0;
u_int8_t fxCurrentLevel = 0;

void loop()
{
    unsigned long t = millis();

    display.clearDisplay();
    display.setCursor(0, 0);

    switch(mode){
        case 0:
            patternCloud(t, t - last_t);
            display.println("cloud");
            break;
        case 1:
            pattern_classic(t, t - last_t);
            display.println("classic");
            break;
        case 2:
            simpleWave(t, t - last_t);
            display.println("wave");
            break;
        case 3:
            pattern_rainbow_blast(t);
            display.println("rainbow");
            break;
        case 4:
            pattern_variable_pulses(t);
            display.println("v.pluse");
            break;
        default:
            break;
    }

    sparkle(t);

    memcpy(leds, layer0, sizeof(leds));

    nblend(leds, layer1, STRAND_LENGTH, 128);

    debouncer.update();
    debouncer2.update();

    if (debouncer2.fell()){
        cycleModes(t);
    }

    // post process

    if (debouncer.fell())
    {
        dim = !dim;
    }

    if (dim){
        display.println("Dim");
        fxTargetLevel = 255;
    }
    else
    {
        display.println("Normal");
        fxTargetLevel = 0;
    }

    char buff[50];
    sprintf(buff, "Post %d", knobLeft.read());
    display.println(buff);

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

    // pattern_warm_white(t);
    //

    //leds[t - last_t] = CRGB::Teal;
    last_t = t;

    display.display();
    FastLED.show(); // display this frame

    // pwm outputs
    CHSV color2 = rgb2hsv_approximate(leds[0]);
    //analogWrite(7, 256);
    digitalWrite(7, HIGH);

    FastLED.delay(2);
}
