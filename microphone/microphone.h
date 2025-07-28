// header file for microphone
#ifndef MICROPHONE_H
#define MICROPHONE_H

void initializeMicrophone();
void processMicrophoneData();
void onMicReceive();

extern const int amplitudeThreshold;
extern const int gain;
extern const int bufferSize;
extern const int inputLength;
extern const char* vocalization_labels[];
// extern const int num_vocalizations;

#endif
