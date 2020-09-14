#include <Arduino.h>
#include <U8g2lib.h>

#include <SPI.h>
#include <Wire.h>

#include "State.h"

void initDisplay();

void updateDisplay(State &state);