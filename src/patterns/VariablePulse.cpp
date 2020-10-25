#include "VariablePulse.h"

VariablePulse::VariablePulse(int idx) : Pattern(idx, "V Pulse")
{
    pMetadata = new paramMetadata[6];
    pMetadata[0] = {NORMAL, 128};
    pMetadata[1] = {OCTAVE, 96};
    pMetadata[2] = {NONE, 0};
    pMetadata[3] = {NONE, 0};
    pMetadata[4] = {NONE, 0};
    pMetadata[5] = {NONE, 0};
}

// return a cyclical (sine wave) value between min and max
float VariablePulse::cycle(float t, float period, float min, float max)
{
    return .5 * (min + max) - .5 * (max - min) * cos(t / period * (2 * PI));
}

// blend linearly between a and b, fraction=0 returns a, fraction=1 returns b
float VariablePulse::blend(float a, float b, float fraction)
{
    return (1 - fraction) * a + fraction * b;
}

int VariablePulse::brightness_to_value(float brightness, float min_brightness)
{
    return blend(min_brightness, 1., brightness) * 255;
}

void VariablePulse::fill(CRGB *leds, long numLEDs, long t, long dt, State &state)
{
    float clock = t / 1000.0 * octave(state, 1);
    // int BASE_HUE = 175;
    float density_scale_factor = numLEDs / 36.;

    float period = 30; // s
    float peakedness = 3;
    float min_pulse_width = 5. * density_scale_factor;
    float max_pulse_width = numLEDs * 2.5;
    float crawl_speed_factor = 1; // around 1 is the sweet spot; changing this too much seems to look much worse
    float min_brightness = .05;

    // cycle in the inverse space to balance short vs. long pulses better
    float pulse_width = 1. / cycle(clock, period, 1. / min_pulse_width, 1. / max_pulse_width);
    float crawl_offset = crawl_speed_factor * clock;
    for (int i = 0; i < numLEDs; i++)
    {
        float brightness = cycle(numLEDs - i + crawl_offset * pulse_width, pulse_width, 0, 1);
        brightness = pow(brightness, peakedness);
        int value = brightness_to_value(brightness, min_brightness);
        // leds[i] = CHSV(state.globalParams[0], 255, value);

        value = (value * 1.25) - 64;
        if (value > 255)
            value = 255;
        if (value < 0)
            value = 0;

        if (value > 8)
        {
            leds[i] = ColorFromPalette(state.currentPalette, value, value);
        }
        else
        {
            leds[i] = CRGB::Black;
        }
    }
}