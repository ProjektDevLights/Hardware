#include "GlobalStrip.h"
#include "Arduino.h"
#include "ArduinoJson.h"

#include "Utils.h"

#ifndef Control_h
#define Control_h
class Control
{
public:
    Control();
    void loop();

private:
    WiFiClient client;
    String readData();
    DynamicJsonDocument stringToJSON(String raw);
    void fading(int delay);
    void fadeTo(RGB color, int timeout);
    void initStrip();
};

#endif