#include "Adafruit_NeoPixel.h"
#include "Arduino.h"

#include "RGB.h"
#include "Storage.h"
#include "StripPattern.h"

#ifndef Strip_h
#define Strip_h

typedef void (*lengthCallback)(void);

class Strip
{
public:
    Strip();
    bool setLength(int length, lengthCallback callback);
    bool setLength(int length);
    void showPattern(StripPattern pattern);
    void clear();
    void setBrightness(int brightness);

private:
    void showColor(RGB color);
};
#endif