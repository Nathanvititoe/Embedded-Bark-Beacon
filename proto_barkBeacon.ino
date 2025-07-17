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
*/

#include <Bounce2.h>
#include <ArduinoBLE.h>

// TODO: Figure out config storage that's board compatible

// configure BLE service,
BLEService configService("180C");                 // UUID
BLEByteCharacteristic bitmask("2A56", BLEWrite);  // define the received byte
const uint8_t defaultConfig = 0b0000;

// temporary constant until ml logic is applied
const char* predictedVocalization = "Howl";

const int motor = 9;          // D9
const int pushBtn = 5;        // D5
Bounce debouncer = Bounce();  // btn debouncer
uint8_t bitmaskConfig;        // init config variable

// arduino initial setup
void setup() {
  Serial.begin(9600);  // init serial monitor
  BLE.begin();         // init BLE and configure
  bitmaskConfig = defaultConfig; // init config
  // set name and service ID
  BLE.setLocalName("Bark Beacon");
  BLE.setDeviceName("Bark Beacon");
  BLE.setAdvertisedService(configService);

  configService.addCharacteristic(bitmask);  // add characteristic to service
  BLE.addService(configService);             // add service
  BLE.advertise();                           // broadcast BLE service
  Serial.println("BLE Setup Complete.");

  // init components
  pinMode(motor, OUTPUT);     // init motor
  pinMode(pushBtn, INPUT);    // init pushbtn
  debouncer.attach(pushBtn);  // attach debouncer to pushBtn
  debouncer.interval(25);     // debounce every 25ms

  // test components
  digitalWrite(motor, HIGH);
  delay(2000);

  // reset motor
  digitalWrite(motor, LOW);
  Serial.println("Setup Complete.");
}

// continuous arduino loop
void loop() {
  BLE.poll();            // non-blocking poll to the BLE connection for new data
  debouncer.update();    // update debouncer
  receiveBLETransmit();  // maintain BLE connection

  // if theres a "vocalization"
  if (debouncer.fell()) {
    Serial.println("Dog Vocalization detected");

    // ----------------------------------------------
    // TODO: apply ML model logic here
    // ----------------------------------------------

    isCorrectable(predictedVocalization);  // check if vocalization should be corrected
  }
}

// logic for maintaining/receiving BLE transmissions
void receiveBLETransmit() {
  if (bitmask.written()) {
    uint8_t newBitmaskConfig = bitmask.value();  // read received bitmask value
    Serial.print("New config received: ");
    Serial.println(newBitmaskConfig, BIN);

    // if bitmask is valid, set global to match
    if (validateBitmask(newBitmaskConfig)) {
      bitmaskConfig = newBitmaskConfig;
      // TODO: persist config here
    }
  }
}

// check if a vocalization is marked as correctable in the saved bitmask config
void isCorrectable(String label) {
  Serial.print(label);
  Serial.println(" detected.");
  uint8_t classBit = getClassBit(label);  // convert bit into string label

  if ((bitmaskConfig & classBit) != 0) {
    applyCorrection();  // apply correction through the collar
    Serial.print("Vocalization Detected, Correction applied for: ");
    Serial.println(label);
  } else {
    Serial.print("Vocalization Detected, no correction applied for: ");
    Serial.println(label);
  }
}

// map each class to a bit value
uint8_t getClassBit(String label) {
  label.toLowerCase();  // ensure label is lowercase
  if (label == "bark") return 0b1000;
  if (label == "growl") return 0b0100;
  if (label == "whine") return 0b0010;
  if (label == "howl") return 0b0001;
  return defaultConfig;  // if uknown, default to no corrections
}


// Applying a Correction includes turning on the vibration motor for 2 seconds
void applyCorrection() {
  Serial.println("Correction applied.");
  // 2s correction
  digitalWrite(motor, HIGH);  // vibrate 2s
  delay(2000);                // 2s motor duration

  // reset motor (turn off)
  digitalWrite(motor, LOW);
}

// return true if bitmask is within the expected range, false if not
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
  Serial.println("Invalid Bitmask Config, defaulting to no corrections.");
  return false;
}
