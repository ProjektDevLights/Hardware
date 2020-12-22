#include "Adafruit_NeoPixel.h"
#include "Arduino.h"

#include "RGB.h"
#include "Storage.h"
#include "StripPattern.h"
#include "Utils.h"
#include "math.h"
#ifndef Strip_h
#define Strip_h

class Strip
{
public:
    Strip();
    bool setLength(int length, std::function<void()> callback);
    bool setLength(int length);
    void showPattern(StripPattern pattern);
    void clear();
    void setBrightness(int brightness);

private:
    void showColor(RGB color);
    void showGradient(RGB first, RGB second);
};
#endif