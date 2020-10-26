#include "VariablePulse.h"

VariablePulse::VariablePulse() : Pattern("V Pulse")
{
    pMetadata = new paramMetadata[6];
    pMetadata[0] = {NORMAL, 128};
    pMetadata[1] = {OCTAVE, 96};
    pMetadata[2] = {NORMAL, 30};  // period (s)
    pMetadata[3] = {NORMAL, 15};  // min width
    pMetadata[4] = {NORMAL, 255}; // max width
    pMetadata[5] = {NORMAL, 255}; // palette range
}

// return a cyclical (sine wave) value between min and max
float VariablePulse::cycle(float t, float period, float min, float max)
{
    return .5 * (min + max) - .5 * (max - min) * cos(t / period * (2 * PI));
}

void VariablePulse::fill(CRGB *leds, long numLEDs, long t, long dt, State &state)
{
    float clock = t / 1000.0 * octave(state, 1);
  
    float period = patternParam(state,2); // s
    float peakedness = 3;
    float min_pulse_width = numLEDs * constrain(patternParam(state, 3),10,255)  / 100.0;
    float max_pulse_width = numLEDs * patternParam(state, 4) / 100.0;
    float crawl_speed_factor = 1; // around 1 is the sweet spot; changing this too much seems to look much worse
    float min_brightness = .05;

    // cycle in the inverse space to balance short vs. long pulses better
    float pulse_width = 1. / cycle(clock, period, 1. / min_pulse_width, 1. / max_pulse_width);
    float crawl_offset = crawl_speed_factor * clock;
    for (int i = 0; i < numLEDs; i++)
    {
        float brightness = cycle(numLEDs - i + crawl_offset * pulse_width, pulse_width, 0, 1);
        brightness = pow(brightness, peakedness);
        int value = constrain(brightness, min_brightness, 1) * 255; //brightness_to_value(brightness, min_brightness);

        value = constrain((value * 1.25) - 64, 0, 255);
        if (value > 8)
        {
            leds[i] = ColorFromPalette(state.currentPalette, value * patternParam(state, 5) / 255.0, value);
        }
        else
        {
            leds[i] = CRGB::Black;
        }
    }
}