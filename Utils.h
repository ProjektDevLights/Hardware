#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "ArduinoJson.h"
#include "StringSplitter.h"
#include "ArduinoJson.h"
#include "RGB.h"

#include "StripPattern.h"

#ifndef Utils_h
#define Utils_h
class Utils
{
public:
    static String ipToString(IPAddress ip);
    static StripPattern generatePattern(String pattern, JsonArray colorArray, int timeout);
    static String generatePatternString(int patternNr);
    static RGB generateColor(String color);
    static DynamicJsonDocument stringToJSON(String raw);
    static int stepRound(float number);
    static int generateStep(int start, int end, int time, int delay = 1);
};

#endif