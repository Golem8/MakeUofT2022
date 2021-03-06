#ifndef DEVICE_SEARCH_H
#define DEVICE_SEARCH_H

#include <vector>       // std::vector
#include <string>       // std::string
#include <BluetoothSerial.h>


std::vector<std::pair<std::string, std::string>> getDiscoverableDevices(BluetoothSerial& SerialBT);



#endif
