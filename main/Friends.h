#ifndef FRIENDS_H
#define FRIENDS_H

#include <string>
using namespace std;
#include <vector>

bool checkForFriendFromRawInputs(vector<pair<string , string >> devices);
bool register_friend_to_device(string friend_name, string device_name);
void add_friend_device(string device_name, string MAC);
void remove_by_device_name(string device_name);
bool is_device_name_in_list(string device_name);
bool is_MAC_in_list(string MAC);

struct friend_data {
    string name;
    string MacAddress;
    string deviceName;
};

#endif
