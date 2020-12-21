#include "ESP8266WiFi.h"
#include "Arduino.h"

#ifndef WifiManager_h
#define WifiManager_h

class WifiManager
{
public:
    static void connect(String ssid, String password);
    static IPAddress getIp();
};

#endif