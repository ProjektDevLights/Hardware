#include "Arduino.h"
#include "ArduinoJson.h"
#include "ESP8266WiFi.h"
#include "RGB.h"
#include "Storage.h"
#include "StringSplitter.h"
#include "StripPattern.h"

#ifndef Utils_h
#define Utils_h

class Utils {
   public:
    static String ipToString(IPAddress ip);
    static DynamicJsonDocument stringToJSON(String raw);
    static RGB stringToRGB(String color);
    static std::vector<RGB> jsonArrayToVector(JsonArray array);
    static String patternIntToString(int patternNr);
    static StripPattern dataToStripPattern(String pattern, JsonArray colorArray,
                                           int timeout);
    static int RGBToHue(RGB color);
    static std::vector<RGB> generatePixelsOff(int length);
    static std::vector<RGB> generatePixelsColor(int length, RGB color);
    static std::vector<RGB> generatePixels(int length, StripPattern pattern,
                                           int startIndex = 0);
    static RGB interpolateColor(RGB first, RGB second, int step, int steps);
    static int interpolateValue(int first, int second, int step, int steps);
    static void blink(int runs, int secs_on, int secs_off);
};

#endif