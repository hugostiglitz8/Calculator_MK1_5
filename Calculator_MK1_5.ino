#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <bluefruit.h>
#include "Fraction.h"
#include "DecimalMath.h"  // Add decimal math support
#include "Keypad.h"
#include "Display.h"

// Mode switch pin
const int MODE_SWITCH_PIN = A5;

// these live in Display.ino:
extern const int displayX, displayY, displayW, displayH;
extern String    lastDisplayCalc, lastDisplayHistory, lastDisplayAlternate;
extern bool      displayNeedsUpdate;
extern Adafruit_ILI9341 tft;

// Mode switching variables
Mode lastMode = MODE_CALCULATOR;
bool lastSwitchState = HIGH;
unsigned long lastSwitchTime = 0;
const unsigned long DEBOUNCE_DELAY = 50; // ms

void setup() {
  Serial.begin(115200);
  
  // Initialize mode switch pin
  pinMode(MODE_SWITCH_PIN, INPUT);
  
  // Initialize display
  tft.begin();
  tft.setRotation(4);
  
  // Initialize keypad
  initKeypad();
  
  // Initialize calculator display
  drawInitialDisplay();
  
  // Initialize numberpad (BLE)
  initNumberpad();
  
  // Check initial mode
  checkModeSwitch();
  
  Serial.println("Ready - Calculator/Numberpad with Decimal Math");
  Serial.print("Initial mode: ");
  Serial.println(currentMode == MODE_CALCULATOR ? "Calculator" : "Numberpad");
  
  // Test decimal math on startup
  Serial.println("Testing decimal math:");
  Decimal test1 = Dec("21.03");
  Decimal test2 = Dec("14.567");
  Decimal result = test1 + test2;
  Serial.print("21.03 + 14.567 = ");
  Serial.println(result.toString());
}

void loop() {
  keyScan();
  detectZeroHold();
   detectSlashHold(); 
  checkModeSwitch();
  //debugSwitch(); 
  processKeyBuffer();
  handleDisplay();
  handleModeDisplay();
}

/*void debugSwitch() {
  static unsigned long lastDebug = 0;
  
  // Print switch state every 500ms
  if (millis() - lastDebug > 500) {
    bool switchState = digitalRead(MODE_SWITCH_PIN);
    Serial.print("Switch pin 30: ");
    Serial.print(switchState ? "HIGH" : "LOW");
    Serial.print(" | Current mode: ");
    Serial.println(currentMode == MODE_CALCULATOR ? "CALC" : "NUMPAD");
    lastDebug = millis();
  }
}*/

// Update the checkModeSwitch() function:
void checkModeSwitch() {
  bool currentSwitchState = digitalRead(MODE_SWITCH_PIN);
  
  // Debounce the switch
  if (currentSwitchState != lastSwitchState) {
    lastSwitchTime = millis();
  }
  
  if ((millis() - lastSwitchTime) > DEBOUNCE_DELAY) {
    // The switch state has been stable for the debounce period
    // Now check if the stable state is different from current mode
    
    Mode newMode;
    if (currentSwitchState == HIGH) {
      // Switch is HIGH (connected to 3.3V) = Calculator mode
      newMode = MODE_CALCULATOR;
    } else {
      // Switch is LOW (connected to ground) = Numberpad mode
      newMode = MODE_NUMBERPAD;
    }
    
    // Only update if mode actually changed
    if (newMode != currentMode) {
      currentMode = newMode;
      
      Serial.print("Mode changed to: ");
      Serial.println(currentMode == MODE_CALCULATOR ? "Calculator" : "Numberpad");
      
      if (currentMode == MODE_CALCULATOR) {
        drawInitialDisplay();
        stopNumberpadAdvertising(); // Stop advertising in calculator mode
      } else {
        startNumberpadAdvertising(); // Start advertising in numberpad mode
      }
      
      displayNeedsUpdate = true;
    }
  }
  
  lastSwitchState = currentSwitchState;
}