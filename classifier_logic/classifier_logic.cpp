#include "classifier_logic.h"
#include "MicroTFLite.h"
#include "../audio_classifier/audio_classifier.h"
#include "../correctionConfig/correctionConfig.h"

// extern global vars from main
extern const char* vocalization_labels[];
extern const int num_vocalizations;

/*
* Initialize the Audio Classification Model
*/
void initialize_classifier(uint8_t* tensorArena, int tensorArenaSize) {
  Serial.println();
  Serial.println("Init model...");
  bool success = ModelInit(audio_classifier, tensorArena, tensorArenaSize);
  Serial.println("Init complete?");
  if (!success) {
    Serial.println("Model initialization failed!");
    while (true)
      ;
  }
  // Serial.println("Init model..");
  // if (!ModelInit(audio_classifier, tensorArena, tensorArenaSize)) {
  //   Serial.println("Model initialization failed!");
  //   while (true)
  //     ;
  // }
  Serial.println("Model initialization Complete.");
  ModelPrintMetadata();  // [IMPORTANT] use the logged tensor arena size to adjust the global arena size
  ModelPrintTensorQuantizationParams();
  ModelPrintTensorInfo();
  Serial.println();
}

/*
*  Fill input tensor with normalized data
*/
void populateInputTensor(short* buffer, int length) {
  Serial.println("Populating Input Tensor...");
  for (int i = 0; i < length; i++) {
    ModelSetInput((float)(buffer[i] >> 8), i);  // convert 16-bit input to 8-bit for model
  }
}

/* 
*  Run inference and return status
*/
bool getInference() {
  return ModelRunInference();
}

/*
* Get top prediction index and its confidence
*/
int getTopPrediction(float& confidence) {
  confidence = 0.0f;
  int index = 0;
  for (int i = 0; i < num_vocalizations; i++) {
    float value = ModelGetOutput(i);
    if (value > confidence) {
      confidence = value;
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
    Serial.print(ModelGetOutput(i) * 100.0f, 2);
    Serial.println("%");
  }
  Serial.println();
}