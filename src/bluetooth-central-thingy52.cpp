/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "application.h"
#line 1 "/Users/jaredwolff/Circuit_Dojo/bluetooth-central-thingy52/src/bluetooth-central-thingy52.ino"
/*
 * Project bluetooth-central-thingy52
 * Description: Particle Mesh + Thingy52 Example
 * Author: Jared Wolff
 * Date: 12/4/2019
 */

// System mode for better Bluetooth Performance
void scanResultCallback(const BleScanResult *scanResult, void *context);
void onDisconnect(const BlePeerDevice& peer, void* context);
void onTemperatureRecieved(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context);
void setup();
void loop();
#line 9 "/Users/jaredwolff/Circuit_Dojo/bluetooth-central-thingy52/src/bluetooth-central-thingy52.ino"
SYSTEM_MODE(MANUAL);

// List of UUIDs in String format
#define THINGY_UUID "EF680100-9B35-4933-9B10-52FFA9740042"
#define THINGY_CHAR_TEMP "EF680201-9B35-4933-9B10-52FFA9740042"

// List of UUIDs
BleUuid thingyCharTemp = BleUuid(THINGY_CHAR_TEMP);

// List of characteristics
BleCharacteristic temperatureCharacteristic;

// List of important objects for BLE operations
BleAddress thingyAddress = BleAddress("ff:ff:ff:ff:ff:ff");
BlePeerDevice thingy;

// Logger
SerialLogHandler logHandler(115200, LOG_LEVEL_ERROR, {
    { "app", LOG_LEVEL_INFO }, // enable all app messages
});

// scanResultCallback() used to fetch scan results
void scanResultCallback(const BleScanResult *scanResult, void *context) {

  // Filtering using UUID
	// BleUuid uuids[4];
	// int uuidsAvail = scanResult->advertisingData.serviceUUID(uuids,sizeof(uuids)/sizeof(BleUuid));

  // for(int i = 0; i < uuidsAvail; i++) {
  //   String name = uuids[i].toString();

  //   if(name == THINGY_UUID) {
  //     Log.info("Found!");
  //   }
  // }


  // Filtering using name
  // Check if name matches Thingy52
  String name = scanResult->advertisingData.deviceName();

  char buf[128];
  name.toCharArray(buf, sizeof(buf));

  Log.info("Device name %s", buf);

  // Check if they're equal
  if( name == "Thingy" ) {

    // Collect the uuids showing in the advertising data
    BleUuid uuids[20];
    int uuidsAvail = scanResult->advertisingData.serviceUUID(uuids,sizeof(uuids)/sizeof(BleUuid));

    // Used to store the string representation of the UUID recieved
    char uuidString[256];

    // Loop over all available UUIDs
    // For tile devices there should only be one
    for(int i = 0; i < uuidsAvail; i++){
      uuids[i].toString(uuidString,sizeof(uuidString));
      Log.info("UUID: %s", uuidString);
    }

    // Stop scannning
    BLE.stopScanning();

    // Set address
    thingyAddress = scanResult->address;

  }

}

void onDisconnect(const BlePeerDevice& peer, void* context){

  Log.info("Disconnected");

  // Reset address
  thingyAddress = BleAddress("ff:ff:ff:ff:ff:ff");

}

void onTemperatureRecieved(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context){

  if(len == 2) {
    Log.info("Temperature: %d.%d", data[0], data[1]);
  }



}

// setup() runs once, when the device is first turned on.
void setup() {
  // Set disconnect handler
  BLE.onDisconnected(onDisconnect,NULL);

}

// loop() runs over and over again, as quickly as it can execute.
void loop() {

  // Run the scan
  if(!(thingyAddress == BleAddress("ff:ff:ff:ff:ff:ff")) && !thingy.connected()){
    // Connnect!
    thingy = BLE.connect(thingyAddress);

  } else if( !thingy.connected() ) {
    // Scan
    BLE.scan(scanResultCallback, NULL);

  }

  // Validate characteristic
  if( thingy.connected() && !temperatureCharacteristic.valid()) {

    Log.info("Gettng temperature characteristic.");

    // Get the char
    thingy.getCharacteristicByUUID(temperatureCharacteristic,thingyCharTemp);

    // Check if it's good!
    if(temperatureCharacteristic.valid()) Log.info("Valid!");
    else Log.info("Invalid!");

    // Set the callback
    temperatureCharacteristic.onDataReceived(onTemperatureRecieved,NULL);

  }

}