#include <Arduino.h>
#include <PDM.h>
#include "microphone.h"
#include "../correctionConfig/correctionConfig.h"
#include "../classifier_logic/classifier_logic.h"

// external globals from main
extern const int amplitudeThreshold;
extern const int gain;
extern const int bufferSize;
extern short sampleBuffer[];
extern volatile int samplesRead;
extern const char* vocalization_labels[];
extern const int num_vocalizations;

static bool isRecording = false;
static int recordingIndex = 0;
static short recordingBuffer[INPUT_LENGTH];  // match INPUT_LENGTH

/*
* configure microphone settings
*/
void initializeMicrophone() {
  PDM.onReceive(onMicReceive);         // callback for when input is detected
  PDM.setBufferSize(bufferSize);       // set buffer size at 512 bytes
  PDM.setGain(gain);                   // set sensitivity
  bool success = PDM.begin(1, 16000);  // 1 channel, 16kHz
  while (!success) {
    Serial.println("Retrying mic init...");
    delay(1000);
    success = PDM.begin(1, 16000);
  }
}

/*
* callback for analog signal inputs
*/
void onMicReceive() {
  int bytes = PDM.available();
  PDM.read(sampleBuffer, bytes);  // read the sample from the buffer
  samplesRead = bytes / 2;        // 2 bytes per sample
}

/*
* how to process microphone inputs
*/
void processMicrophoneData() {
  if (samplesRead == 0) return;

  bool triggered = false;
  for (int i = 0; i < samplesRead; i++) {
    if (abs(sampleBuffer[i]) > amplitudeThreshold) {
      triggered = true;
      break;
    }
  }

  if (triggered && !isRecording) {
    isRecording = true;
    recordingIndex = 0;
    Serial.println("Threshold triggered, Capturing...");
  }

  if (isRecording) {
    for (int i = 0; i < samplesRead && recordingIndex < INPUT_LENGTH; i++) {
      recordingBuffer[recordingIndex++] = sampleBuffer[i];
    }

    if (recordingIndex >= INPUT_LENGTH) {
      Serial.println("Window full → Running inference...");
      for (int i = 0; i < INPUT_LENGTH; i++) {
        modelSetInput(quantizeSample(recordingBuffer[i]), i);
      }

      if (!modelRunInference()) {
        Serial.println("Inference failed");
      } else {
        printModelOutputs();
        float confidence = 0.0f;
        int topClass = getTopPrediction(confidence);
        if (confidence > 0.7f) {
          Serial.print("Predicted: ");
          Serial.print(vocalization_labels[topClass]);
          Serial.print(" (");
          Serial.print(confidence * 100.0f, 1);
          Serial.println("%)");
          isCorrectable(vocalization_labels[topClass]);
        } else {
          Serial.println("Low confidence, skipping correction.");
        }
      }

      isRecording = false;
      recordingIndex = 0;
      samplesRead = 0;
    }
  }

  samplesRead = 0;
}
