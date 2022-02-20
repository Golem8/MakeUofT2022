#include "DeviceSearch.h"
#include <BluetoothSerial.h>
#include <vector>       // std::vector
#include <utility>      // std::pair, std::make_pair
#include <string>       // std::string

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#define BT_DISCOVER_TIME  10000 // 1000 sometimes worked, sometimes failed. 10000 should ensure reliable detection

// vector of string pairs of the form (name, address)
std::vector<std::pair<std::string, std::string>> devicesFound;



void deviceFoundCb(BTAdvertisedDevice* foundDevice) {
  
  std::string name = foundDevice->getName();
  std::string addr = foundDevice->getAddress().toString();
  devicesFound.push_back(std::make_pair(name,addr));
  // Serial.printf("Found a device asynchronously: %s\n", foundDevice->toString().c_str());
}


std::vector<std::pair<std::string, std::string>> getDiscoverableDevices(){
    BluetoothSerial SerialBT;
    SerialBT.begin("Armband"); //Bluetooth device name
  
    devicesFound.clear(); //start with an empty device list

  if (SerialBT.discoverAsync(deviceFoundCb)) {
    delay(BT_DISCOVER_TIME);
    SerialBT.discoverAsyncStop();
    
    return devicesFound;
  } else {
    Serial.println("Error on discoverAsync f.e. not workin after a \"connect\"");
  }
}
