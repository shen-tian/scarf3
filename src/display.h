#include <Arduino.h>
#include <U8g2lib.h>

#include <SPI.h>
#include <Wire.h>

#include "State.h"

static char dialXBM[][26] = {{
  0x00, 0x00, 0x00, 0x00, 0xE0, 0x00, 0x18, 0x03, 0xE4, 0x04, 0xE4, 0x04, 
  0x02, 0x08, 0x02, 0x08, 0x02, 0x08, 0x04, 0x04, 0x04, 0x04, 0x18, 0x03, 
  0xE0, 0x00, },{
  0x00, 0x00, 0x00, 0x00, 0xE0, 0x00, 0x18, 0x03, 0xC4, 0x04, 0xC4, 0x05, 
  0x02, 0x08, 0x02, 0x08, 0x02, 0x08, 0x04, 0x04, 0x04, 0x04, 0x18, 0x03, 
  0xE0, 0x00, },{
  0x00, 0x00, 0x00, 0x00, 0xE0, 0x00, 0x18, 0x03, 0x84, 0x04, 0x84, 0x05, 
  0x02, 0x09, 0x02, 0x08, 0x02, 0x08, 0x04, 0x04, 0x04, 0x04, 0x18, 0x03, 
  0xE0, 0x00, },{
  0x00, 0x00, 0x00, 0x00, 0xE0, 0x00, 0x18, 0x03, 0x04, 0x04, 0x84, 0x05, 
  0x02, 0x0B, 0x02, 0x08, 0x02, 0x08, 0x04, 0x04, 0x04, 0x04, 0x18, 0x03, 
  0xE0, 0x00, },{
  0x00, 0x00, 0x00, 0x00, 0xE0, 0x00, 0x18, 0x03, 0x04, 0x04, 0x04, 0x05, 
  0x02, 0x0B, 0x02, 0x0B, 0x02, 0x08, 0x04, 0x04, 0x04, 0x04, 0x18, 0x03, 
  0xE0, 0x00, }};


void initDisplay();

void updateDisplay(State &state);