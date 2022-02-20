#include <iostream>
#include <string>
using namespace std;
#include <list>

#include "Friends.h"

vector<friend_data> friends_database = {};
list<string> detectable_friends = {};

// CURRENT LIST FUNCTIONS
// check if the queried MAC is already in the list
bool is_MAC_in_list(string MAC){
    for (auto it = detectable_friends.begin(); it != detectable_friends.end(); it++){
        if (*it == MAC){
            return true;
        }
    }
    return false;
}

void place_MAC_in_list(string MAC){
    if (is_MAC_in_list(MAC)){
        return; // mac already in list dont try to place
    }
    // now place the mac in the back of the list
    detectable_friends.push_back(MAC);
}

bool remove_MAC_in_list(string MAC){
    if (!is_MAC_in_list){
        return false; // mac not found, return false
    }
    // mac is there somewhere, find and delete it
    detectable_friends.remove(MAC);
    return true;
}

// inputs given as either 2 strings or an object/pair with the device name and its address
// GLOBAL DATABASE FUNCTIONS
void add_friend_device(string device_name, string MAC){
    if (is_MAC_in_vector(MAC)){
        return;
    }
    friend_data tempFriend;
    tempFriend.deviceName = device_name;
    tempFriend.MacAddress = MAC;
    friends_database.push_back(tempFriend);
}

// later functionality
bool register_friend_to_device(string friend_name, string device_name){
    // friend found or not found variable
    bool found = false;
    int friends_database_size = friends_database.size();

    if (friends_database_size == 0){
        return false; // cant do lookup on empty database
    }
    for (int i = 0; i <friends_database_size; i++){
        if (friends_database[i].deviceName == device_name){
            friends_database[i].name = friend_name;
            return true;
        }
    } 
    return false; // no device found with that name
}

// clear device name from friends database
void remove_by_device_name(string device_name){
    for (int i = 0; i < friends_database.size(); i++){
        if (friends_database[i].deviceName == device_name){
            friends_database.erase(friends_database.begin()+i+1);
        }
    }
}

// true if device is found in a list
bool is_device_name_in_vector(string device_name){
    for (int i = 0; i < friends_database.size(); i++){
        if (friends_database[i].deviceName == device_name){
            return true; // found it
        }
    }
    return false; // name not found
}

bool is_MAC_in_vector(string MAC){
    for (int i = 0; i < friends_database.size(); i++){
        if (friends_database[i].MacAddress == MAC){
            return true; //found
        }
    }
    return false; // not found
}
