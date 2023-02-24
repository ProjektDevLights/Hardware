#include "Arduino.h"
#include "ArduinoOTA.h"
#include "EEPROM.h"
#include "ESP8266WiFi.h"
#include "GlobalStrip.h"
#include "HttpClient.h"
#include "Storage.h"
#include "StripPattern.h"
#include "Utils.h"
#include "WifiManager.h"

#ifndef Setup_h
#define Setup_h

class Setup {
   public:
    static bool run();

   private:
    static void init();
    static bool first();
    static bool restart();
};
#endif