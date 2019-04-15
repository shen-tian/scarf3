
#include <FastLED.h>

#ifdef __AVR__
#include <avr/power.h>
#endif

/**
   This part is for hardware related config. It allows you to spec the board
   you are using, along with the LED strand length/brightness. The brightness is
   typically used to limit max power draw: at 60mA per LED (full white), you can
   power at most 8 LEDs from the 500mA 5V pin on boards like the Trinket/Arduino
   Nano. As the strand gets longer, you should use brightness to limit max current
   draw. However, the typical pattern won't ever reach full white on all LEDs, so
   the actual max current varies. It's probably best established via direct
   measurement. An alternative reason to limit brightness is to improve battery
   life.

   Current configs:

 *  * Arduino Nano, use pin 6
 *  * Adafruit Trinket 5V 16Mhz, use pin 0
*/

#define PIN 0

#define STRAND_LENGTH 144
#define BRIGHTNESS 255

/**
   Whether the pattern is mirrored, or reversed. This is useful for scarfs where
   the LEDs are all daisy chained. An alternative is to have the center pixel
   being the first one, and split the d-out line down either sides
*/
//#define MIRRORED
#define REVERSED

#if defined(MIRRORED)
#define ARM_LENGTH (STRAND_LENGTH / 2)
#else
#define ARM_LENGTH STRAND_LENGTH
#endif

CRGB leds[STRAND_LENGTH];

CRGB layer0[STRAND_LENGTH];
CRGB layer1[STRAND_LENGTH];

void setup()
{
    Serial.begin(9600);
    Serial.println("Scarf OS 3.0");

    //FastLED.addLeds<WS2811, PIN, GRB>(leds, STRAND_LENGTH).setCorrection( TypicalLEDStrip );
    FastLED.addLeds<APA102, 3, 4, BGR>(leds, STRAND_LENGTH).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);
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
        leds[i] = CHSV(per_pixel_hue_jump * i + crawl_speed_factor * clock, 255, 255);
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

    for (byte pix = 0; pix < ARM_LENGTH; pix++)
    {
        // location of the pixel on a 0-RENDER_RANGE scale.
        byte dist = pix * 255 / ARM_LENGTH;

        byte delta = modDist(dist, pulse);
        // linear ramp up of brightness, for those within 1/8th of the reference point
        float value = max(255 - 6 * delta, 64);

        // hue selection. Mainly driving by c, but with some small shifting along
        // the length of the strand.

        // sweep of a subset of the spectrum.
        float x = color / 255. + pix * 128. / ARM_LENGTH;
        if (x >= 1)
            x -= 1.;

        float hue = 255 * HUE_START + cubicwave8(x) * (HUE_END - HUE_START);

        byte loc = pix;
#if defined(REVERSED)
        loc = ARM_LENGTH - 1 - pix;
#endif

        layer0[loc] = CHSV(hue, 255 * SATURATION, value);
    }
}

void pattern_warm_white(long t)
{
    fill_solid(leds, STRAND_LENGTH, CRGB(255, 147, 41));
    fade_video(leds, STRAND_LENGTH, 192);
}

void patternCloud(long t)
{
    CRGB buff[STRAND_LENGTH];
    for (int i = 0; i < STRAND_LENGTH; i++)
    {
        uint8_t value = inoise8(t / 8, (1000 + i - t / 64) * 10);
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

    fade_video(layer0, STRAND_LENGTH, 128);
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

int mode = 1;
long time_last_switch = 0;

void cycleModes(long t)
{
    if ((t - time_last_switch) > 2000)
    {
        mode++;
        time_last_switch = t;
    }

    if (mode == 2)
        mode = 0;
}

long last_t = 0;

void loop()
{
    unsigned long t = millis();

    //cycleModes(t);

    // if (mode == 0)
    // {
    //     pattern_rainbow_blast(t);
    // }
    // else
    // {
    //     pattern_classic(t);
    // }
    patternCloud(t);
    //pattern_classic(t);
    sparkle(t);

    memcpy(leds, layer0, sizeof(leds));

    nblend(leds, layer1, STRAND_LENGTH, 168);

    
    //pattern_warm_white(t);
    //pattern_rainbow_blast(t);

    //leds[t - last_t] = CRGB::Teal;
    last_t = t;

    FastLED.show(); // display this frame
    FastLED.delay(30);
}