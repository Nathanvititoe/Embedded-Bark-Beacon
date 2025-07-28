#include <Arduino.h>
#include "correctionConfig.h"

// external globals
extern const int motor;
extern uint8_t bitmaskConfig;
extern const uint8_t defaultConfig;

/*
* init vibration motor
*/
void initializeVibrationMotor() {
  pinMode(motor, OUTPUT);
  digitalWrite(motor, LOW);
}

/*
*  check if a vocalization is marked as correctable in the saved bitmask config
*/
void isCorrectable(String label) {
  Serial.print(label);
  Serial.println(" detected.");
  uint8_t classBit = getClassBit(label);  // convert bit into string label

  if ((bitmaskConfig & classBit) != 0) {
    applyCorrection(false);  // apply correction through the collar
    Serial.print("Vocalization Detected, Correction applied for: ");
    Serial.println(label);
  } else {
    Serial.print("Vocalization Detected, no correction applied for: ");
    Serial.println(label);
  }
}

/* 
*  turn on vibration motor for 2s
*/
void applyCorrection(bool isInitial) {
  // 2s correction
  digitalWrite(motor, HIGH);  // vibrate 2s
  delay(2000);                // 2s motor duration

  // reset motor (turn off)
  digitalWrite(motor, LOW);
  if (isInitial) {
    Serial.println("Setup Test Successful");
  } else {
    Serial.println("Correction applied.");
  }
}


/*
*  map each class to a bit value
*/
uint8_t getClassBit(String label) {
  label.toLowerCase();  // ensure label is lowercase
  if (label == "bark") return 0b1000;
  if (label == "growl") return 0b0100;
  if (label == "whine") return 0b0010;
  if (label == "howl") return 0b0001;
  return defaultConfig;  // if uknown, default to no corrections
}


/* 
* return true if bitmask is within the expected range, false if not
*/
bool validateBitmask(uint8_t bitmask) {
  if (bitmask <= 0b1111) {
    Serial.println("--------------------");
    Serial.print("read config hex: ");
    Serial.println(bitmask, HEX);
    Serial.print("read config bitmask: ");
    Serial.println(bitmask, BIN);  // print tested bitmask
    Serial.println("--------------------");
    return true;
  }
  // if out of range, set global to default, return false
  bitmaskConfig = defaultConfig;
  Serial.println("Invalid Bitmask Config, using default Config.");
  return false;
}