// header file for ML Model Inferences and Logic
#ifndef CLASSIFIER_LOGIC_H
#define CLASSIFIER_LOGIC_H

#include <Arduino.h>
void initialize_classifier();
void populateInputTensor(short* buffer, int length);
bool getInference();
int getTopPrediction(float& confidence);
void printModelOutputs();

extern const char* vocalization_labels[];
extern const int num_vocalizations;

#endif