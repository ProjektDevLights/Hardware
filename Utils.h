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
    static RGB interpolateColor(RGB first, RGB second, int step, int steps);
    static int interpolateValue(int first, int second, int step, int steps);
    static std::vector<RGB> generatePixels(int length, StripPattern pattern, int startIndex = 0);
    static std::vector<RGB> generatePixelsColor(int length, RGB color);
    static std::vector<RGB> generatePixelsOff(int length);
};

#endif