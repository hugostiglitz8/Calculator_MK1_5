// Display.h

#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "Keypad.h"  // for Mode enum

// (these are defined in Display.ino)
extern const int        displayX, displayY, displayW, displayH;
extern String           lastDisplayCalc, lastDisplayHistory, lastDisplayAlternate;
extern bool             displayNeedsUpdate;
extern Adafruit_ILI9341 tft;
extern const uint16_t   backgroundColor;

void drawInitialDisplay();
void updateDisplay();
void handleDisplay();
void clearDisplayArea();
void handleModeDisplay();

#endif // DISPLAY_H