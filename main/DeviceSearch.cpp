/*
  Adapted from example written by Thomas M. (ArcticSnowSky)
  See commit: https://github.com/espressif/arduino-esp32/commit/41c372c143a9585d67110782e735d80813f1889f
  
*/

#include "DeviceSearch.h"
#include <vector>       // std::vector
#include <utility>      // std::pair, std::make_pair
#include <string>       // std::string

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#define BT_DISCOVER_TIME  5000 // 1000 sometimes worked, sometimes failed. 5k should ensure reliable detection

// vector of string pairs of the form (name, address)
std::vector<std::pair<std::string, std::string>> devicesFound;


void deviceFoundCb(BTAdvertisedDevice* foundDevice) {
  std::string name = foundDevice->getName();
  std::string addr = foundDevice->getAddress().toString();
  devicesFound.push_back(std::make_pair(name,addr));
  // Serial.printf("Found a device asynchronously: %s\n", foundDevice->toString().c_str());
}


std::vector<std::pair<std::string, std::string>> getDiscoverableDevices(BluetoothSerial& SerialBT){
  devicesFound.clear(); //start with an empty device list

  // perform the asyncronous search for nearby bt devices. Wait for a specified amount of time before returning to loop()
  // Any devices found will call the callback, which stores them in devicesFound, which is retruned by this function
  if (SerialBT.discoverAsync(deviceFoundCb)) {
    delay(BT_DISCOVER_TIME);
    SerialBT.discoverAsyncStop();  
    SerialBT.discoverClear();

  } else {
    Serial.println("Error on discoverAsync f.e. not workin after a \"connect\"");
  }

    return devicesFound;
}
