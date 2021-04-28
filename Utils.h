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
    static std::vector<RGB> generatePixels(int length, StripPattern pattern, int startIndex = 0);
    static std::vector<RGB> offPixels(int length);

private:
    static std::vector<RGB> generatePixelsSingle(int length, RGB color);
    static std::vector<RGB> generatePixelsGradient(int length, RGB first, RGB second);
    static std::vector<RGB> generatePixelsRunner(int length, RGB color, int startIndex);
};

#endif