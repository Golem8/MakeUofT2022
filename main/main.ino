

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
   // SerialBT.disconnect();
    
  }
}


const int greenLED = 5; // IO34 on GPIO corresponds to pin5 ADC-CH6
const int redLED  = 6; // IO35 on GPIO corresponds to pin6 ADC-CH7
const int vibratePIN = 8; //IO33 on GPIO corresponds to pin8 ADC-CH5
// using them as output pins anyways so dont care about the input number variation thing between 0 and 4095

// GPIO4 corresponds to pin13 which will be read in as T0 by the esp library

void setup() {
  Serial.begin(115200);

  // Registering a callback function (in case)
  // https://techtutorialsx.com/2018/12/09/esp32-arduino-serial-over-bluetooth-client-connection-event/
  SerialBT.register_callback(bt_event_callback);

  SerialBT.begin("Armband"); //Bluetooth device name
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(vibratePIN, OUTPUT);
  digitalWrite(vibratePIN, LOW);
  digitalWrite(greenLED, LOW);
  digitalWrite(redLED, LOW);
}

void friendDetectedPulser(){
    // values in ms
    int onDur = 2000;
    int offDur = 1000;
    int repetitions = 5;
    // 2 up 1 down, 5 times
    for (int i = 0; i < repetitions; i++){
      digitalWrite(greenLED, HIGH);
      digitalWrite(redLED, HIGH);
      digitalWrite(vibratePIN, HIGH);
      delay(onDUR);

      digitalWrite(greenLED, LOW);
      digitalWrite(redLED, LOW);
      digitalWrite(vibratePIN, LOW);
      delay(offDur);
    }
}

bool pairingAccept(){
  // return true if we accept by pressing during the interval of 2 seconds
  // store the smallest value and compare at the end of the interval with the threshold, to be determined
  // values decrease upon touch, threshold of 50
  int threshold = 50;
  int wait_for_authenticate = 5000;
  int touch_value = INT_MAX; // start at the highest value
  for (int i = 0; i < wait_for_authenticate; i++){
    if (touchRead(T0) < touch_value){
      touch_value = touchRead(T0);
    }
    delay(1);
  }
  if (touch_value < threshold){
    return true;
  }
  return false;
}

// Half the time, the ESP32 will be checking for nearby matches (5 seconds)
// The other half of the time, process incoming pair requests
void loop() {
// update global variable
if (friend_detection_flag){
  // call the routine to handle the vibrations and the flashing of the LED
  // this is for the registered friend being discovered
  friendDetectedPulser();
}

  std::vector<std::pair<std::string, std::string>> devices = getDiscoverableDevices(SerialBT);
   for(auto it = devices.begin(); it != devices.end(); it++){
     Serial.printf("Name: %s                Addr: %s\n", (*it).first.c_str(), (*it).second.c_str());
   }

   //pairing not possible when getting discoverable devices, so leave a long delay here to allow time for any incoming pairs
  delay(5000);
}
