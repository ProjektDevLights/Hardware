#include "Arduino.h"
#include "ESP8266WiFi.h"

#ifndef WifiManager_h
#define WifiManager_h

class WifiManager {
   public:
    static void connect(String ssid, String password);
    static IPAddress getIp();
};

#endif