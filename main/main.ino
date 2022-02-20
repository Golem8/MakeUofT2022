

#include <Arduino.h>
#include "DeviceSearch.h"
#include <esp_bt_main.h>
#include <esp_bt_device.h>
#include <BluetoothSerial.h>
#include "Friends.h"
#include <string>

BluetoothSerial SerialBT;

//globals to get mac addr of connecting user. MAX of 20 paired devices
uint8_t pairedDeviceBtAddr[20][6];
//this array is a global output holder for the bda2str function
char bda_str[18];

bool friend_detection_flag = false;

// converts bonded device address (array of 6 chars) to mac address seperated by colons
// taken from https://github.com/espressif/arduino-esp32/commit/38c4c0610846b7193e908b474e2c8db06ae981ba
char *bda2str(const uint8_t* bda, char *str, size_t size)
{
  if (bda == NULL || str == NULL || size < 18) {
    return NULL;
  }
  sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x",
          bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
  return str;
}


// Richard Li - 2020, public domain code
void BTAuthCompleteCallback(boolean success)
{
  if (success)
  {
    std::string clientDeviceName = ""; // for now, simply pass empty string for devicename. Mac addr is enough to uniquely identify a device
    std::string clientMacAddress = "";

    // adapted from from https://github.com/espressif/arduino-esp32/commit/38c4c0610846b7193e908b474e2c8db06ae981ba
    int count = esp_bt_gap_get_bond_device_num();
    if(!count) Serial.println("No bonded device found.");
    
    // adapted from from https://github.com/espressif/arduino-esp32/commit/38c4c0610846b7193e908b474e2c8db06ae981ba
    esp_err_t tError =  esp_bt_gap_get_bond_device_list(&count, pairedDeviceBtAddr);
    if(ESP_OK == tError) {
      for(int i = 0; i < count; i++) {
        Serial.print("Found bonded device # "); Serial.print(i); Serial.print(" -> ");
        Serial.println(bda2str(pairedDeviceBtAddr[i], bda_str, 18));

        clientMacAddress = bda2str(pairedDeviceBtAddr[i], bda_str, 18);
        
        if (pairingAccept()) {
          Serial.printf("Accepted pairing via capacitive touch, adding mac %s to database", clientMacAddress.c_str());
          add_friend_device(clientDeviceName, clientMacAddress);
         // place_MAC_in_list()
        }else{
          Serial.printf("REJECTED pairing via capacitive touch, NOT ADDING mac %s to database", clientMacAddress.c_str());
        }

        SerialBT.unpairDevice(pairedDeviceBtAddr[i]);
        // code to unpair the device. After registering the device, we want to unpair it
        esp_err_t tError = esp_bt_gap_remove_bond_device(pairedDeviceBtAddr[i]);
        if(ESP_OK == tError) {
          Serial.print("Removed bonded device # "); 
        } else {
          Serial.print("Failed to remove bonded device # ");
        }
        Serial.println(i);
        
      }        
    }
    // disconnect from the device after unpairing from it
    SerialBT.disconnect();
  }
  else
  {
    Serial.println("Pairing failed, rejected by user!!");
  }
}


const int greenLED = 5; // IO34 on GPIO corresponds to pin5 ADC-CH6
const int redLED  = 7; // IO32 on GPIO corresponds to pin6 ADC-CH7
const int vibratePIN = 8; //IO33 on GPIO corresponds to pin8 ADC-CH5
// using them as output pins anyways so dont care about the input number variation thing between 0 and 4095

// GPIO4 corresponds to pin13 which will be read in as T0 by the esp library

void setup() {
  Serial.begin(115200);

  // Registering a callback function for when a user pairs
  // Using IconEV's example code 
  // https://github.com/espressif/arduino-esp32/commit/38c4c0610846b7193e908b474e2c8db06ae981ba
  SerialBT.onAuthComplete(BTAuthCompleteCallback);

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
      delay(onDur);

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
  digitalWrite(redLED, HIGH);
  for (int i = 0; i < wait_for_authenticate; i++){
    if (touchRead(T0) < touch_value){
      touch_value = touchRead(T0);
    }
    delay(1);
  }
  digitalWrite(redLED, LOW);

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
