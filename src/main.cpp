#include <FastLED.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <avdweb_Switch.h>

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

CRGB leds[STRAND_LENGTH];

// 0 is base, 1 is foreground, 2 is overlay
CRGB layer0[STRAND_LENGTH];
CRGB layer1[STRAND_LENGTH];
CRGB layer2[STRAND_LENGTH];

Switch pushButton = Switch(BUTTON_PIN);

void setup()
{
    Serial.begin(9600);
    Serial.println("Scarf OS 3.0");

    #if defined(LED_WS2812)
    FastLED.addLeds<WS2811, LED_DATA_PIN, GRB>(leds, STRAND_LENGTH).setCorrection( TypicalLEDStrip );
    #endif

    #if defined(LED_APA102)
    FastLED.addLeds<APA102, LED_DATA_PIN, LED_CLOCK_PIN, BGR>(leds, STRAND_LENGTH).setCorrection(TypicalLEDStrip);
    #endif

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
    float crawl_speed_factor = 1000;

    for (int i = 0; i < STRAND_LENGTH; i++)
    {
        layer2[STRAND_LENGTH - 1 - i] = CHSV(per_pixel_hue_jump * i + crawl_speed_factor * clock, 255, 255);
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

void pattern_classic(long t)
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

void pattern_standby(long t)
{
    fill_solid(leds, STRAND_LENGTH, CRGB::Black);

    if ((t / 1000) % 5 == 0){
        leds[0] = CRGB::Red;
        leds[STRAND_LENGTH / 2] = CRGB::Red;
        leds[STRAND_LENGTH - 1] = CRGB::Red;
    }
    fade_video(leds, STRAND_LENGTH, 192);
}

float u = 1;
float damp = .99;

float uprightness()
{
    if (!imuPresent)
        return .5;

    imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

    float dist = euler.z() - 90;
    if (dist < -180)
        dist += 360;

    float reading = (90 - abs(dist)) / 90;

    u = damp * u + (1 - damp) * reading;

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
            buff[i] = CHSV(hue, sat, value);
        else
            buff[i] = CRGB::Black;

    }
    nblend(layer0, buff, STRAND_LENGTH, 255);

    //fade_video(layer0, STRAND_LENGTH, 128);
}

void sparkle(long t, long dt, boolean birth)
{
    int n = 50 * STRAND_LENGTH;
    if (birth)
        n = STRAND_LENGTH;
    int spot = random16(n);
    if (spot < STRAND_LENGTH)
        layer1[spot] = CHSV(0, 0, 255);

    fadeUsingColor(layer1,STRAND_LENGTH,CRGB(230, 239, 245));

    // leds[t - last_t] = CRGB(255,0,0);
    // last_t = t;
}

long last_t = 0;

double party = 0;
double rest = 0;

bool standby = false;

void loop()
{
    unsigned long t = millis();

    pushButton.poll();
    if (pushButton.doubleClick())
    {
        double upright = uprightness();
        if ((party > 4) || rest > 4 || standby)
        {
            party = 0;
            rest = 0;
            standby = false;
        }
        else
        {
        if (upright > .25)
            party = 1000;
        else if (upright > -.1)
            rest = 255;
        else
            standby = true;
        }
    }

    if (!standby)
    {
        pattern_rainbow_blast(t);
        patternCloud(t, t - last_t);
        //pattern_classic(t);
        sparkle(t, t - last_t, pushButton.on());

        memcpy(leds, layer0, sizeof(leds));
        nblend(leds, layer1, STRAND_LENGTH, 128);

        nblend(leds, layer2, STRAND_LENGTH, min(party, 255));

        party = .98 * party;

        if (rest > 0) pattern_warm_white(t);
    }
    else
    {
        pattern_standby(t);
    }
    //
    //leds[t - last_t] = CRGB::Teal;
    last_t = t;

    FastLED.show(); // display this frame
    FastLED.delay(10);
}
