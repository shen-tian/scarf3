#ifndef PALETTES_H
#define PALETTES_H

#include <FastLED.h>

const TProgmemRGBGradientPalette_byte aurora_gpp[] = {
    0, 120, 30, 28,
   51, 120, 30, 28,
   51, 150, 62, 32,
  102, 150, 62, 32,
  102, 206,154, 55,
  153, 206,154, 55,
  153,  79,133, 56,
  204,  79,133, 56,
  204, 103, 70, 96,
  255, 103, 70, 96};

  const TProgmemRGBGradientPalette_byte bhw1_06_gp[] = {
    0, 184,  1,128,
  160,   1,193,182,
  219, 153,227,190,
  255, 255,255,255};

  // Gradient palette "GMT_ocean_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/gmt/tn/GMT_ocean.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 36 bytes of program space.

const TProgmemRGBGradientPalette_byte GMT_ocean_gp [] = {
    0,   0,  0,  0,
   31,   0,  1,  1,
   63,   0,  1,  4,
   95,   0, 19, 42,
  127,   0, 79,138,
  159,  15,144,112,
  191,  91,233, 89,
  223, 155,244,158,
  255, 242,255,255};

#endif