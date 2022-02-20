

#include <Arduino.h>
#include "DeviceSearch.h"
#include <esp_bt_main.h>
#include <esp_bt_device.h>
#include <BluetoothSerial.h>
#include "Friends.h"
#include <string>

BluetoothSerial SerialBT;


void BTAuthCompleteCallback(boolean success)
{
  if (success)
  {
    std::string clientDeviceName = ""; // for now, simply pass empty string for devicename. Mac addr is enough to uniquely identify a device
    std::string clientMacAddress = "";
    const uint8_t* clientMacAddressArr = esp_bt_dev_get_address();

    // build mac address from the returned array: Source: https://www.youtube.com/watch?v=fxvoNpiqipQ
    for (size_t i = 0; i < 6; i++)
    {
      char pair[3];
      // parse the ith bits into characters. pair of chars is 3 long due to null terminator.
      // Left pad with 0's, taking a minimum of 2 characters
      sprintf(pair, "%02x", (int)clientMacAddressArr[i]);
      
      // add the pair of characters to the mac
      clientMacAddress += pair;
      if (i < 5) clientMacAddress += ":";
    }

    Serial.printf("Built client mac: %s\n", clientMacAddress.c_str());

    // store this string into db
    add_friend_device(clientDeviceName, clientMacAddress);
    Serial.println("Pairing success!!");
  }
  else
  {
    Serial.println("Pairing failed, rejected by user!!");
  }
}

void setup() {
  Serial.begin(115200);




  // Registering a callback function for when a user pairs
  // Using IconEV's example code 
  // https://github.com/espressif/arduino-esp32/commit/38c4c0610846b7193e908b474e2c8db06ae981ba
  SerialBT.onAuthComplete(BTAuthCompleteCallback);

  SerialBT.begin("Armband"); //Bluetooth device name

}

// Half the time, the ESP32 will be checking for nearby matches (5 seconds)
// The other half of the time, process incoming pair requests
void loop() {
  std::vector<std::pair<std::string, std::string>> devices = getDiscoverableDevices(SerialBT);
   for(auto it = devices.begin(); it != devices.end(); it++){
     Serial.printf("Name: %s                Addr: %s\n", (*it).first.c_str(), (*it).second.c_str());
   }

   //pairing not possible when getting discoverable devices, so leave a long delay here to allow time for any incoming pairs
  delay(5000);
}
