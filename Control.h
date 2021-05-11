#include "Arduino.h"
#include "ArduinoJson.h"
#include "ESPNtpClient.h"
#include "GlobalStrip.h"
#include "Utils.h"

#ifndef Control_h
#define Control_h
class Control {
   public:
    void setup();
    void loop();

   private:
    WiFiClient client;
    bool isCustom;
    String readData();
    void sendPattern();
};

#endif