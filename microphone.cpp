#include <Arduino.h>
#include <PDM.h>
#include "microphone.h"
#include "correctionConfig.h"

// external globals from main
extern const int amplitudeThreshold;
extern const int gain;
extern const int bufferSize;
extern short sampleBuffer[];
extern volatile int samplesRead;
// =========== TEST VAR =================
String simulatedPrediction = "growl";

// configure microphone settings
void initializeMicrophone() {
  PDM.onReceive(onMicReceive);         // callback for when input is detected
  PDM.setBufferSize(bufferSize);       // set buffer size at 512 bytes
  PDM.setGain(gain);                     // set sensitivity
  bool success = PDM.begin(1, 16000);  // 1 channel, 16kHz
  if (!success) {
    Serial.println("Failed to start PDM mic!");
    while (1)
      ;  // stop everything
  } else {
    Serial.println("Mic ready");
  }
}

// callback for analog signal inputs
void onMicReceive() {
  int bytes = PDM.available();
  PDM.read(sampleBuffer, bytes);  // read the sample from the buffer
  samplesRead = bytes / 2;        // 2 bytes per sample
}

// how to process microphone inputs
void processMicrophoneData() {
  if (samplesRead == 0) return;

  bool triggered = false;
  int peak = 0;

  // loop throough samples and find the peak
  // trigger ML inference if threshold is broken
  for (int i = 0; i < samplesRead; i++) {
    int absSample = abs(sampleBuffer[i]);
    if (absSample > peak) peak = absSample;
    if (absSample > amplitudeThreshold) triggered = true;
  }

  samplesRead = 0;

  if (triggered) {
    Serial.print("Peak: ");
    Serial.println(peak);
    // ============================
    // CALL ML INFERENCE LOGIC HERE
    // =============================
    isCorrectable(simulatedPrediction);  // simulate ML
  }
}
