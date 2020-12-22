#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "ArduinoJson.h"
#include "StringSplitter.h"
#include "ArduinoJson.h"

#include "StripPattern.h"

#ifndef Utils_h
#define Utils_h
class Utils
{
public:
    static String ipToString(IPAddress ip);
    static StripPattern generatePattern(String pattern, JsonArray colorArray);
    static DynamicJsonDocument stringToJSON(String raw);
    static int stepRound(float number);
};

#endif