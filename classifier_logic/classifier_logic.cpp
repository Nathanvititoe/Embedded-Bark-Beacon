#include <ArduTFLite.h>
#include "classifier_logic.h"
#include "../audio_classifier/audio_classifier.h"  // model array header

// extern global vars from main
extern const char* vocalization_labels[];
extern const int num_vocalizations;
/*
* Initialize the Audio Classification Model
*/
void initialize_classifier(uint8_t* tensorArena, int tensorArenaSize) {
  Serial.println();
  bool modelInitialized = modelInit(audio_classifier, tensorArena, tensorArenaSize);
  if (!modelInitialized) {
    Serial.println("Model initialization failed!");
    while (true) {}
  }
  Serial.println("Model initialization complete.");
}

/*
* Convert audio samples to the normalized range the model expects
*/
uint8_t quantizeSample(int16_t sample) {
  // Convert 16-bit PCM to 0..255
  float normalized = (sample + 32768) / 65535.0f;
  return (uint8_t)(normalized * 255);
}

/*
*  Fill input tensor with normalized data
*/
void populateInputTensor(short* buffer, int length) {
  int samplesToProcess = min(length, INPUT_LENGTH);

  Serial.print("Buffer length: ");
  Serial.print(length);
  Serial.print(", processing ");
  Serial.print(samplesToProcess);
  Serial.println(" samples");

  extern TfLiteTensor* tflInputTensor;  // get input tensor from ArduTFLite

  // Use tensor quantization params
  float scale = tflInputTensor->params.scale;
  int zero_point = tflInputTensor->params.zero_point;

  for (int i = 0; i < samplesToProcess; ++i) {
    float value = static_cast<float>(buffer[i]);  
    int32_t quantized = static_cast<int32_t>(value / scale) + zero_point;

    // keep in valid uint8 range
    quantized = max(0, min(255, quantized));

    modelSetInput((uint8_t)quantized, i);
  }

  // pad with zero
  for (int i = samplesToProcess; i < INPUT_LENGTH; ++i) {
    modelSetInput(zero_point, i);  // Fill with silence
  }
}

/* 
*  Run inference and return status
*/
bool getInference() {
  if (!modelRunInference()) {
    Serial.println("Inference failed!");
    return false;
  }
  return true;
}

/*
* Get top prediction index and its confidence
*/
int getTopPrediction(float& confidence) {
  confidence = 0.0f;
  int index = 0;
  for (int i = 0; i < num_vocalizations; i++) {
    float val = modelGetOutput(i);
    if (val > confidence) {
      confidence = val;
      index = i;
    }
  }
  return index;
}

/*
* Print all model outputs
*/
void printModelOutputs() {
  for (int i = 0; i < num_vocalizations; i++) {
    Serial.print(vocalization_labels[i]);
    Serial.print(": ");
    Serial.print(modelGetOutput(i));
    Serial.println("%");
  }
  Serial.println();
}