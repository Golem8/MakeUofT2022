/*
  Adapted from example written by Thomas M. (ArcticSnowSky)
  See commit: https://github.com/espressif/arduino-esp32/commit/41c372c143a9585d67110782e735d80813f1889f
  
*/

#include <Arduino.h>
#include "DeviceSearch.h"



void setup() {
  Serial.begin(115200);
  
}

void loop() {
  std::vector<std::pair<std::string, std::string>> devices = getDiscoverableDevices();
   for(auto it = devices.begin(); it != devices.end(); it++){
     Serial.printf("Name: %s                Addr: %s\n", (*it).first.c_str(), (*it).second.c_str());
   }
  delay(4000);
}
