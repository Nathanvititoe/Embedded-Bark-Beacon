#include <Arduino.h>
#include <ArduinoBLE.h>
#include "bluetooth.h"
#include "correctionConfig.h"

// external globals
extern BLEService configService;
extern BLEByteCharacteristic bitmask;
extern uint8_t bitmaskConfig;

// configure the BLE service
void initializeBLE() {
  BLE.begin();  // init BLE and configure
  BLE.setLocalName("Bark Beacon");
  BLE.setDeviceName("Bark Beacon");
  BLE.setAdvertisedService(configService);

  configService.addCharacteristic(bitmask);  // add characteristic to service
  BLE.addService(configService);             // add service
  BLE.advertise();                           // broadcast BLE service

  Serial.println("BLE Setup Complete.");
}


// if updated data is received when polling, handle it
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