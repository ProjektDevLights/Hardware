#include "Arduino.h"
#include "EEPROM.h"
#include "Storage.h"
#include "ESP8266WiFi.h"
#include "HttpClient.h"

#include "WifiManager.h"
#include "Utils.h"
#include "GlobalStrip.h"
#include "StripPattern.h"

#ifndef Setup_h
#define Setup_h

class Setup
{
public:
    static void run();

private:
    static void init();
    static void first();
    static void restart();
};
#endif