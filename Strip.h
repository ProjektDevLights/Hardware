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
    void showCurrentPattern(bool noFade = false);
    void showPattern(StripPattern pattern, bool noFade = false);
    void showColor(RGB color, bool noFade = false);
    void showCustom(JsonArray colors);
    void showOff(bool noFade = false);

    bool setLength(int length);
    bool setBrightness(int brightness, bool silent = false);

    void update();
    void stopRunning();

private:
    int activePattern = -1;
    int brightness = 255;
    bool curDirectionForward = true;
    int curFadeIndex = 0;
    int curRunIndex = 20;
    StripPattern curPattern;
    int hue = 0;
    int length = 150;
    unsigned long lastUpdate = 0;

    void showPixelArray(std::vector<RGB> colors);
    void fadeToPixelArray(std::vector<RGB> from, std::vector<RGB>);

    void fadeUpdate();
    void rainbowUpdate();
    void runnerUpdate();

    std::vector<RGB> readStrip();
    int getFirstRunLed();

    uint32_t RGBToPixelColor(RGB color);
    RGB pixelColorToRGB(uint32_t color);
};
#endif