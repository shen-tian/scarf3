#include "Fire2012.h"

Fire2012::Fire2012() : Pattern("Fire")
{
    pMetadata = new paramMetadata[6];
    pMetadata[0] = {NORMAL, 128};
    strcpy(pMetadata[0].label, "LVL");
    pMetadata[1] = {OCTAVE, 96};
    strcpy(pMetadata[1].label, "SPD");
    pMetadata[2] = {NORMAL, 120}; 
    strcpy(pMetadata[2].label, "SRK");
    pMetadata[3] = {NORMAL, 96};
    strcpy(pMetadata[3].label, "COL");
    pMetadata[4] = {NONE, 128};
    pMetadata[5] = {NONE, 0};
}

// Fire2012 by Mark Kriegsman, July 2012
// as part of "Five Elements" shown here: http://youtu.be/knWiGsmgycY
//// 
// This basic one-dimensional 'fire' simulation works roughly as follows:
// There's a underlying array of 'heat' cells, that model the temperature
// at each point along the line.  Every cycle through the simulation, 
// four steps are performed:
//  1) All cells cool down a little bit, losing heat to the air
//  2) The heat from each cell drifts 'up' and diffuses a little
//  3) Sometimes randomly new 'sparks' of heat are added at the bottom
//  4) The heat from each cell is rendered as a color into the leds array
//     The heat-to-color mapping uses a black-body radiation approximation.
//
// Temperature is in arbitrary units from 0 (cold black) to 255 (white hot).
//
// This simulation scales it self a bit depending on NUM_LEDS; it should look
// "OK" on anywhere from 20 to 100 LEDs without too much tweaking. 
//
// I recommend running this simulation at anywhere from 30-100 frames per second,
// meaning an interframe delay of about 10-35 milliseconds.
//
// Looks best on a high-density LED setup (60+ pixels/meter).
//
//
// There are two main parameters you can play with to control the look and
// feel of your fire: COOLING (used in step 1 above), and SPARKING (used
// in step 3 above).
//
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 55, suggested range 20-100 
#define COOLING  55

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120

void Fire2012::fill(CRGB *leds, long numLEDs, long t, long dt, State &state)
{
    random16_add_entropy( random());

    byte cooling = patternParam(state, 3);
    byte sparking = patternParam(state, 2);
   

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < numLEDs; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((cooling * 10) / numLEDs) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= numLEDs - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < sparking ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < numLEDs; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( state.currentPalette, colorindex, patternParam(state, 0));
      leds[j] = color;
    }
}