#ifndef FRIENDS_H
#define FRIENDS_H

#include <string>
using namespace std;
#include <vector>
#include <list>

struct friend_data {
    string name;
    string MacAddress;
    string deviceName;
};

// friends database
extern vector<friend_data> friends_database;

// new to proximity friends, who have not triggered detection sequence yet
// this will take from the friends_database, and then remove when a friend is detected, add that person back when they leave
extern list<string> detectable_friends;

bool is_MAC_in_list(string MAC); 
void place_MAC_in_list(string MAC);
bool remove_MAC_in_list(string MAC);


bool checkForFriendFromRawInputs(vector<pair<string , string >> devices);
void friendDetected();
bool register_friend_to_device(string friend_name, string device_name);
void add_friend_device(string device_name, string MAC);
void remove_by_device_name(string device_name);
bool is_device_name_in_list(string device_name);
bool is_MAC_in_list(string MAC);
bool is_MAC_in_vector(string MAC);



#endif
