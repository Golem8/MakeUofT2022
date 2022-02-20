

#include <Arduino.h>
#include "DeviceSearch.h"
#include <esp_bt_device.h>
#include <BluetoothSerial.h>
#include "Friends.h"
#include <string>

BluetoothSerial SerialBT;

// callback function for bluetooth events
// Logic & function definiton taken from 
// https://techtutorialsx.com/2018/12/09/esp32-arduino-serial-over-bluetooth-client-connection-event/
void bt_event_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param){

  Serial.println("Event Occurred");

  // If the current event is a new client connection
  if(event == ESP_SPP_SRV_OPEN_EVT){
    Serial.println("Client Connected");
    std::string clientMacAddress = ""
    const uint8_t* clientMacAddressArr = esp_bt_dev_get_address();

    // build mac address from the returned array
    for (size_t i = 0; i < 6; i++)
    {
      clientMacAddress += clientMacAddressArr[i];
      if (i > 5) clientMacAddress += ":";
    }

    Serial.printf("Built client mac: %s\n", clientMacAddress.c_str());
    
  }
}

void setup() {
  Serial.begin(115200);

  // Registering a callback function (in case)
  // https://techtutorialsx.com/2018/12/09/esp32-arduino-serial-over-bluetooth-client-connection-event/
  SerialBT.register_callback(bt_event_callback);

  SerialBT.begin("Armband"); //Bluetooth device name

}

// Half the time, the ESP32 will be checking for nearby matches (5 seconds)
// The other half of the time, process incoming pair requests
void loop() {
  std::vector<std::pair<std::string, std::string>> devices = getDiscoverableDevices(SerialBT);
   for(auto it = devices.begin(); it != devices.end(); it++){
     Serial.printf("Name: %s                Addr: %s\n", (*it).first.c_str(), (*it).second.c_str());
   }
  delay(4000);
}
