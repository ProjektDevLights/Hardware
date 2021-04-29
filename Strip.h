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
    void showPattern(StripPattern pattern, boolean noFade = false);
    void clear();
    void setBrightness(int brightness, boolean silent = false);
    void showColor(RGB color);
    void update();
    void stopRunning();
    void showCustom(JsonArray colors);
    void fadeOff();

private:
    std::vector<RGB> readStrip();
    void fadeToPixelArray(std::vector<RGB> from, std::vector<RGB>);
    void showPixelArray(std::vector<RGB> colors);
    void showGradient(RGB first, RGB second);
    void fadeUpdate();
    void runnerUpdate();
    void rainbowUpdate();
    void setNewGoal();
    void fadeToColor(RGB color);
    uint32_t generateColor(RGB color);
    RGB pixelColorToRGB(uint32_t color);
};
#endif