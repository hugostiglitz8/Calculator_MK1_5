//keyScan.ino

#include "Keypad.h"

// ── Mode (shared) ──
Mode currentMode = MODE_CALCULATOR;

// Keypad matrix pins
const uint8_t colPins[5] = { 2, 3, 4, 5, 28 };
const uint8_t rowPins[5] = { 16, 15, 7, 11, 31 };

// Keymap
const char* keymap[5][5] = {
  { "AC", "x/y", ">", "/", "Del" },
  { "7", "8", "9", "x", "MM" },
  { "4", "5", "6", "-", "round" },
  { "1", "2", "3", "+", "return" },
  { "0", "0", ".", "+", "return" }
};

// ── Buffer state ──
volatile KeyEvent keyBuffer[KEY_BUFFER_SIZE];
volatile uint8_t  keyHead = 0, keyTail = 0;

// ── Zero-hold state ──
unsigned long zeroPressStart = 0;
bool          zeroPressed    = false;
bool          zeroHoldActive = false;
const unsigned long zeroHoldThreshold = 175;  // ms

// ── Slash-hold state (ONLY DEFINED HERE) ──
unsigned long slashPressStart = 0;
bool          slashPressed    = false;
bool          slashHoldActive = false;
const unsigned long slashHoldThreshold = 175;  // ms

// ── Debounce timing ──
volatile uint32_t lastKeyTime[5][5] = { {0} };
const uint32_t    keyDebounceMs     = 5;

// ── Keypad init ──
void initKeypad() {
  for (int i = 0; i < 5; i++) {
    pinMode(rowPins[i], OUTPUT);
    digitalWrite(rowPins[i], HIGH);
    pinMode(colPins[i], INPUT_PULLUP);
  }
}

// ── Matrix scan ──
void keyScan() {
  static bool lastState[5][5] = { false };
  for (uint8_t r = 0; r < 5; r++) {
    digitalWrite(rowPins[r], LOW);
    delayMicroseconds(1);
    for (uint8_t c = 0; c < 5; c++) {
      bool pressed = !digitalRead(colPins[c]);
      // PRESS edge
      if (pressed && !lastState[r][c]) {
        // Zero key hold detection
        if (r == 4 && (c == 0 || c == 1)) {
          zeroPressed    = true;
          zeroPressStart = millis();
        }
        // Slash key hold detection (row 0, col 3)
        else if (r == 0 && c == 3) {
          slashPressed    = true;
          slashPressStart = millis();
        }
        // Regular key press
        else {
          uint8_t nh = (keyHead + 1) % KEY_BUFFER_SIZE;
          if (nh != keyTail) {
            keyBuffer[keyHead].row = r;
            keyBuffer[keyHead].col = c;
            keyHead = nh;
          }
          lastKeyTime[r][c] = millis();
        }
      }
      // RELEASE edge
      if (!pressed && lastState[r][c]) {
        // Zero key release
        if (r == 4 && (c == 0 || c == 1)) {
          unsigned long held = millis() - zeroPressStart;
          if (held < zeroHoldThreshold) {
            uint8_t nh = (keyHead + 1) % KEY_BUFFER_SIZE;
            if (nh != keyTail) {
              keyBuffer[keyHead].row = r;
              keyBuffer[keyHead].col = c;
              keyHead = nh;
            }
          }
          zeroPressed = false;
          zeroHoldActive = false; // Reset when released
        }
        // Slash key release
        else if (r == 0 && c == 3) {
          unsigned long held = millis() - slashPressStart;
          if (held < slashHoldThreshold) {
            // Short press - send normal slash
            uint8_t nh = (keyHead + 1) % KEY_BUFFER_SIZE;
            if (nh != keyTail) {
              keyBuffer[keyHead].row = r;
              keyBuffer[keyHead].col = c;
              keyHead = nh;
            }
          }
          slashPressed = false;
          slashHoldActive = false; // Reset shift mode on release
        }
        lastKeyTime[r][c] = millis();
      }
      lastState[r][c] = pressed;
    }
    digitalWrite(rowPins[r], HIGH);
  }
}

// ── Zero Long-hold detector ──
void detectZeroHold() {
  if (zeroPressed && !zeroHoldActive
      && millis() - zeroPressStart > zeroHoldThreshold) {
    zeroHoldActive = true;
    Serial.println("Zero hold activated - MM/IN mode");
  }
}

// ── Slash Long-hold detector ──
void detectSlashHold() {
  if (slashPressed && !slashHoldActive
      && millis() - slashPressStart > slashHoldThreshold) {
    slashHoldActive = true;
    Serial.println("Slash hold activated - Shift mode");
  }
}

// --- consume & dispatch buffered key events ---
void processKeyBuffer() {
  while (keyTail != keyHead) {
    // make a non-volatile copy of the event
    KeyEvent e;
    e.row = keyBuffer[keyTail].row;
    e.col = keyBuffer[keyTail].col;

    const char* k = keymap[e.row][e.col];
    if (currentMode == MODE_CALCULATOR) {
      handleCalculatorMode(k);
    } else {
      handleNumberpadMode(k);
    }

    displayNeedsUpdate = (currentMode == MODE_CALCULATOR);
    keyTail = (keyTail + 1) % KEY_BUFFER_SIZE;
  }
}