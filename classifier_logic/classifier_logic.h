#ifndef CLASSIFIER_LOGIC_H
#define CLASSIFIER_LOGIC_H

#include <Arduino.h>
#define INPUT_LENGTH 256

void initialize_classifier(uint8_t* tensorArena, int tensorArenaSize);
void populateInputTensor(short* buffer, int length);
bool getInference();
uint8_t quantizeSample(int16_t sample);
int getTopPrediction(float& confidence);
void printModelOutputs();

#endif
