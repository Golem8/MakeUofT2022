#include <string>
using namespace std;
#include <vector>
#include "Friends.h"


// input is given as a vector of pairs, this input is going to be vector of pairs, name then address
// lets make a parser so its ready for later


bool checkForFriendFromRawInputs(vector<pair<string , string >> devices){
    // first lets see if the device name exists in the database already, if so then we have a detection flag
    // iterate through devices list, check if the deviceName matches up with any friends in the friends database
    int device_list_size = devices.size();
    for (int i = 0; i < device_list_size; i++){
        // perform the search
        bool MAC_exists = is_MAC_in_list(devices[i].second);
        if (MAC_exists){
            return true; // there is a friend's MAC in the inputs, later extend this to do something about it
        }
    }
    return false; // friend MAC not found
}

// routine that handles the LED and vibration outputs for a friend being nearby
void friendDetected(){
    // values in ms
    int onDur = 2000;
    int offDur = 1000;
    int repetitions = 5;
    // 2 up 1 down 
}
