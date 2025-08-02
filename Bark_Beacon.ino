// Nathan Vititoe
// Bark Beacon Test Template
/*
This template is built for use with a push btn in place of a microphone to simulate bark events
The LED should be lit up continuously whenever the motor is on, and off whenever the motor is off 


Adding BLE capability to control the motor via BLE using the LightBlue app (Android or iOS)

16 July - 
Set Config (Correctable Vocalizations) by inputting the hex code from the ConfigHexMap 
Push Btn to simulate bark event, hard coded prediction is in the isCorrectable() function
Send config values from lightBlue app in order to determine whether a correction is applied

17July-
Switch to Arduino Nano BLE Sense w/ built in microphone
split logic into their own .h and cpp files

27July -
Created cpp file with c array of my model
Incorporate MicroTFLite to utilize my machine learning model for inferences
Used example file https://github.com/johnosbb/MicroTFLite/tree/main/examples/ArduinoNano33BLE_GestureClassifier
*/

/*
1August - switched off of MicroTFLite due to complications, using raw TF Lite Micro library installed through git clone

*/

#include <ArduinoBLE.h>  // bluetooth
#include <PDM.h>         // built-in microphone
#include <ArduTFLite.h>  // TF Lite for Arduino

#include "audio_classifier/audio_classifier.cpp"  // custom audio classification model
#include "bluetooth/bluetooth.cpp"                // bluetooth logic
#include "microphone/microphone.cpp"              // microphone logic
#include "correctionConfig/correctionConfig.cpp"  // user config and correction logic header
#include "classifier_logic/classifier_logic.cpp"  // embedded logic for the ml model

// TODO: Figure out config storage that's board compatible

// configure BLE service
BLEService configService("180C");                 // UUID
BLEByteCharacteristic bitmask("2A56", BLEWrite);  // define the received byte

// set default user configuration global
// const uint8_t defaultConfig = 0b0000; // no vocalization corrections
const uint8_t defaultConfig = 0b1111;  // all vocalization corrected
uint8_t bitmaskConfig;                 // init config variable

// configure built-in microphone globals
const int sr = 16000;                // mic sample rate
const int numChannels = 1;           // monochannel
const int bufferSize = 512;          // 512 byte buffer
short sampleBuffer[bufferSize / 2];  // 2 bytes per sample
volatile int samplesRead = 0;        // TEST VAR

// [IMPORTANT] ADJUST ARENA SIZE AFTER INITIAL SETUP

// configure audio classifier memory
// [ESTIMATED] Tensor Arena size
constexpr int tensorArenaSize = 8 * 1024;       // 8 kb
alignas(16) byte tensorArena[tensorArenaSize];  // Tensor Arena memory

const char* vocalization_labels[] = { "bark", "growl", "whine", "howl" };                      // Vocalization labels
const int num_vocalizations = (sizeof(vocalization_labels) / sizeof(vocalization_labels[0]));  // Number of vocalizations

// ========= TO BE ADJUSTED ==================
const int amplitudeThreshold = 5000;  // must be *this* loud to trigger
const int gain = 60;                  // adjust for input sensitivity
// ==========================================

// init motor output pin
const int motor = 2;  // D2

/*
* arduino initial setup
*/
void setup() {
  initializeSerial();             // init serial connection
  bitmaskConfig = defaultConfig;  // init config

  // init components
  initializeVibrationMotor();                           // init motor
  initializeMicrophone();                               // init microphone
  initializeBLE();                                      // init bluetooth
  initialize_classifier(tensorArena, tensorArenaSize);  // init audio classifier

  // quick test
  applyCorrection(true);  // test motor w/ correction
  Serial.println("Setup Complete.");
}

/*
* continuous arduino loop
*/
void loop() {
  // keep bluetooth connection open for config changes
  BLE.poll();               // non-blocking poll to the BLE connection for new data
  receiveBLETransmit();     // maintain BLE connection
  processMicrophoneData();  // detect audio
}


/*
* initialize the Serial connection
*/
void initializeSerial() {
  // int serialBaud = 1000000; // 1mb baud for serial connection
  int serialBaud = 500000;   // 500kb baud for test, allow for println
  Serial.begin(serialBaud);  // init Serial
  while (!Serial)            // 5s timeout when waiting for serial
    ;
  Serial.print("Serial initialized at : ");
  Serial.println(serialBaud);
}
