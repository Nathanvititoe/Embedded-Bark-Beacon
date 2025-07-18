// header file for corrections and user configuration
#ifndef CORRECTIONCONFIG_H
#define CORRECTIONCONFIG_H

#include <Arduino.h>  

void initializeVibrationMotor();
void isCorrectable(String label);
void applyCorrection(bool isInitial);
uint8_t getClassBit(String label);
bool validateBitmask(uint8_t bitmask);

extern uint8_t bitmaskConfig;
extern const int motor; 
extern const uint8_t defaultConfig; 

#endif