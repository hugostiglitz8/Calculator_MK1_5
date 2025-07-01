//Keypad.h

#ifndef KEYPAD_H
#define KEYPAD_H

#include <Arduino.h>

// ── Modes ──
enum Mode { MODE_CALCULATOR, MODE_NUMBERPAD };
extern Mode currentMode;

// ── Keypad matrix pins ──
extern const uint8_t colPins[5];
extern const uint8_t rowPins[5];

// ── Key labels ──
extern const char* keymap[5][5];

// ── Key event buffering ──
#define KEY_BUFFER_SIZE 10
struct KeyEvent { uint8_t row, col; };
extern volatile KeyEvent   keyBuffer[KEY_BUFFER_SIZE];
extern volatile uint8_t    keyHead, keyTail;

// ── Zero-hold state ──
extern unsigned long  zeroPressStart;
extern bool           zeroPressed;
extern bool           zeroHoldActive;
extern const unsigned long zeroHoldThreshold;

// ── Slash-hold state (for shift mode) ──
extern unsigned long  slashPressStart;
extern bool           slashPressed;
extern bool           slashHoldActive;
extern const unsigned long slashHoldThreshold;

// ── Debounce timing ──
extern volatile uint32_t lastKeyTime[5][5];
extern const uint32_t    keyDebounceMs;

// ── Calculator state (from CalculatorLogic.ino) ──
extern String calcLine, historyLine, alternateDisplay;
extern bool justCalculated, usedFractionEntry, displayNeedsUpdate;

// ── Display constants (from Display.ino) ──
extern const uint16_t backgroundColor;

// ── Functions ──
void initKeypad();
void keyScan();
void detectZeroHold();
void detectSlashHold();
void processKeyBuffer();

// ── Mode-specific handlers ──
void handleCalculatorMode(const char* key);
void handleNumberpadMode(const char* key);
void initNumberpad();
void startNumberpadAdvertising();
void stopNumberpadAdvertising();

// ── Answer memory functions (from CalculatorLogic.ino) ──
String getStoredAnswer();
bool hasCalculatorAnswer();
void clearStoredAnswer();

#endif // KEYPAD_H