

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


// Adapted from Richard Li - 2020, public domain code
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
        //  place_MAC_in_list(clientMacAddress); // place this to track the running global // NO WE DONT WANT TO TRACK IT, DO SNIP SNAP
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


const int greenLED = 16; // P16
const int redLED  = 17; // P17
const int vibratePIN = 18; // P18
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
    int onDur = 1000;
    int offDur = 500;
    int repetitions = 8;
    // 2 up 1 down, 5 times
    for (int i = 0; i < repetitions; i++){
      digitalWrite(greenLED, HIGH);
      digitalWrite(vibratePIN, HIGH);
      delay(onDur);
      digitalWrite(greenLED, LOW);
      digitalWrite(vibratePIN, LOW);
      delay(offDur);
    }

    // fulfilled our purposes, go back to false
    friend_detection_flag = false;
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

bool check_for_friend(std::vector<std::pair<std::string, std::string>> devices){
  // parse the entire detected list, run each MAC entry with the running list
  int devicesSz = devices.size();
  for (int i = 0; i < devicesSz; i++){
    string currentMAC = devices[i].second;
    //bool didYaFindIt = is_MAC_in_vector(currentMac);
    if (is_MAC_in_vector(currentMAC)){
      //friend_detection_flag = true;
      friendDetectedPulser();
      // need to get the element in the vector that 
      // we have the string, currentMAC, add this to the list and remove from vector
      place_MAC_in_list(currentMAC);
      friends_database.erase(friends_database.begin()+i);
      return true; // friend detected and flipped
    }
  }
  return false; // no friend detected in the list
}

// need to flip from the removed list to the original list
bool check_for_friend_leaving_radius(std::vector<std::pair<std::string, std::string>> devices){
  int devicesSz = devices.size();
  // need to actually iterate through the currently detected friends list and make sure those values exist within current MAC
  // if it exists, do nothing, BUT IF IT DOESNT EXIST THAT MEANS THEY LEFT SO SWAP BACK
  int current_radius_list_size = detectable_friends.size();
  for (auto it = detectable_friends.begin(); it != detectable_friends.end(); it++){
    // for each element in the detectable list, cross reference that with the currently detected MAC list
    bool weGotThisOne = false; // need to know if we hit a match ever
    for (int i = 0; i < devicesSz; i++){
      if ((*it) == devices[i].second){
        weGotThisOne = true;
      }
    }
    if (weGotThisOne){
      // do nothing, we are still in range and picking up a detected device
    }
    else {
      // snap back to the vector, friend no longer detected
      digitalWrite(redLED, HIGH);
      delay(1000);
      digitalWrite(redLED, LOW); // make beep
      // take out of list, add to vector
      string friendMACAddress = *it; // hold the value
      detectable_friends.remove(*it); // remove from list
      std::string empty = "";
      add_friend_device(empty, friendMACAddress); // put back into detector
      }

  }
return true; // idk
}

// Half the time, the ESP32 will be checking for nearby matches (5 seconds)
// The other half of the time, process incoming pair requests
void loop() {


  std::vector<std::pair<std::string, std::string>> devices = getDiscoverableDevices(SerialBT);
   for(auto it = devices.begin(); it != devices.end(); it++){
     Serial.printf("Name: %s                Addr: %s\n", (*it).first.c_str(), (*it).second.c_str());
   }
  check_for_friend(devices);
  check_for_friend_leaving_radius(devices);
   //pairing not possible when getting discoverable devices, so leave a long delay here to allow time for any incoming pairs
  delay(3000);
}
