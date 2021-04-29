#include "Strip.h"

#ifdef __AVR__
#include <avr/power.h>
#endif
#define PIN 5 //D1
#define PIXELFORMAT NEO_GRB + NEO_KHZ800
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
clock_prescale_set(clock_div_1);
#endif

int activePattern;
StripPattern currentPattern;
RGB currentColor;
int currentStartLed;
int currrentDirectionRight;
RGB goalColor;
unsigned long lastUpdate;
int brightness;
int length = 150;
int curInterIndex = 0;

Adafruit_NeoPixel pixels(1000, PIN, PIXELFORMAT);
Strip::Strip()
{
    pixels.begin();
}

void Strip::update()
{
    switch (activePattern)
    {
    case 2:
        fadeUpdate();
        break;
    case 4:
        runnerUpdate();
        break;
    case 5:
        rainbowUpdate();
        break;
    default:
        break;
    }
}

void Strip::stopRunning()
{
    activePattern = -1;
}

void Strip::showPattern(StripPattern pattern, boolean noFade)
{
    /**
     * Patterns:
     * plain: 1
     * fading: 2
     * gradient: 3
     * runner: 4
     * rainbow: 5 
     */

    std::vector<RGB> oldColors = readStrip();
    std::vector<RGB> newColors = Utils::generatePixels(length, pattern, currentStartLed);

    noFade ? showPixelArray(newColors) : fadeToPixelArray(oldColors, newColors);
    currentPattern = pattern;
    activePattern = pattern.pattern;

    Storage::setStripPattern(pattern);
    currentColor = pattern.colors[0];

    goalColor = RGB({255, 0, 0});
}

std::vector<RGB> Strip::readStrip()
{
    std::vector<RGB> arr;
    arr.resize(length);
    for (int i = 0; i < length; i++)
    {
        arr[i] = pixelColorToRGB(pixels.getPixelColor(i));
    }
    return arr;
}

void Strip::fadeToPixelArray(std::vector<RGB> from, std::vector<RGB> to)
{
    std::vector<RGB> oldColors;
    oldColors.resize(to.size());
    int rSteps[length];
    int gSteps[length];
    int bSteps[length];

    int runs = 40;

    for (int i = 0; i < length; i++)
    {
        RGB oldColor = pixelColorToRGB(pixels.getPixelColor(i));
        oldColors[i] = from[i];
        rSteps[i] = Utils::generateStep(from[i].r, to[i].r, runs);
        gSteps[i] = Utils::generateStep(from[i].g, to[i].g, runs);
        bSteps[i] = Utils::generateStep(from[i].b, to[i].b, runs);
        yield();
    }

    for (int i = 0; i < runs; i++)
    {
        for (int j = 0; j < length; j++)
        {
            RGB oldColor = oldColors[j];
            oldColor.r = oldColor.r - rSteps[j];
            oldColor.g = oldColor.g - gSteps[j];
            oldColor.b = oldColor.b - bSteps[j];
            oldColors[j] = oldColor;
            pixels.setPixelColor(j, generateColor(oldColor));
            yield();
        }

        pixels.show();
    }
    for (int i = 0; i < length; i++)
    {
        pixels.setPixelColor(i, generateColor(to[i]));
    }
    pixels.show();
}

void Strip::showPixelArray(std::vector<RGB> colors)
{
    for (int i = 0; i < length; i++)
    {
        pixels.setPixelColor(i, generateColor(colors[i]));
        yield();
    }
    pixels.show();
}

bool Strip::setLength(int length, std::function<void()> callback)
{
    if (setLength(length))
    {
        callback();
        return true;
    }
    return false;
}

bool Strip::setLength(int pLength)
{
    if (length >= 0)
    {
        Storage::setCount(pLength);
        pixels.clear();
        pixels.show();
        delay(10);
        pixels.updateLength(pLength);
        length = pLength;
        delay(10);
        return true;
    }
    return false;
}

void Strip::clear()
{
    pixels.clear();
    pixels.show();
}

void Strip::setBrightness(int b, boolean silent)
{
    if (silent)
    {
        brightness = b;
    }
    else
    {
        int runs = 30;
        int bF = Storage::getBrightness();
        int difference = b - bF;
        int step = Utils::stepRound(difference / runs);

        for (int i = 0; i < runs; i++)
        {
            brightness += step;
            showPattern(currentPattern, true);
            yield();
        }
        brightness = b;
    }
    Storage::setBrightness(b);
}

//private

void Strip::showColor(RGB color)
{
    pixels.clear();
    pixels.fill(generateColor(color), 0, length);
    delay(1);
    pixels.show();
}

uint32_t Strip::generateColor(RGB color)
{
    return pixels.Color((color.r * brightness / 255), (color.g * brightness / 255), (color.b * brightness / 255));
}

void Strip::fadeOff()
{
    std::vector<RGB> before = readStrip();
    std::vector<RGB> after = Utils::offPixels(length);
    fadeToPixelArray(before, after);
    pixels.clear();
    pixels.show();
}

RGB Strip::pixelColorToRGB(uint32_t color)
{
    uint8_t r = color >> 16;
    uint8_t g = color >> 8;
    uint8_t b = color;

    return RGB({r, g, b});
}

void Strip::showCustom(JsonArray colors)
{
    pixels.clear();
    Serial.println(colors.size());
    for (int i = 0; i < colors.size(); i++)
    {
        RGB color = Utils::generateColor(colors.getElement(i));
        Serial.println(color.toString());
        pixels.setPixelColor(i, generateColor(color));
    }
    pixels.show();
}

void Strip::fadeUpdate()
{

    unsigned long currentMillis = millis();
    if (currentMillis - lastUpdate > currentPattern.timeout)
    {

        RGB curCol = Utils::interpolate(currentColor, goalColor, curInterIndex, 256);
        showColor(curCol);
        curInterIndex++;
        lastUpdate = currentMillis;
        Serial.println("-----------------------------");

        Serial.print("show:    ");
        Serial.println(curCol.toString());
        Serial.print("current: ");
        Serial.println(currentColor.toString());
        Serial.print("goal:    ");
        Serial.println(goalColor.toString());

        if (curCol == goalColor)
        {
            curInterIndex = 0;
            currentColor = goalColor;
            setNewGoal();
            Storage::setStripPattern(currentPattern);
        }
    }
}

void Strip::runnerUpdate()
{
    unsigned long currentMillis = millis();
    if (currentMillis - lastUpdate > currentPattern.timeout)
    {
        lastUpdate = currentMillis;
        const int count = Storage::getCount();
        const int leds = ceil((float)count / 15.0f);
        pixels.clear();
        for (int i = currentStartLed; i < currentStartLed + leds; i++)
        {
            pixels.setPixelColor(i, generateColor(currentColor));
        }
        pixels.show();
        if (currrentDirectionRight)
        {
            currentStartLed++;
            if (currentStartLed > count - 1)
            {
                currentStartLed--;
                currrentDirectionRight = false;
            }
        }
        else
        {
            currentStartLed--;
            if (currentStartLed < 0)
            {
                currentStartLed++;
                currrentDirectionRight = true;
            }
        }
    }
}

void Strip::rainbowUpdate()
{

    unsigned long currentMillis = millis();
    if (currentMillis - lastUpdate > currentPattern.timeout)
    {
        Serial.println("rainbow");
        Serial.println(goalColor.toString());
        lastUpdate = currentMillis;
        pixels.fill(generateColor(currentColor), 0, length);
        pixels.show();
        currentColor = goalColor;
        setNewGoal();
    }
}

void Strip::setNewGoal()
{
    if (currentColor == goalColor)
    {
        if (goalColor == RGB({255, 0, 0}))
        {
            goalColor = RGB({255, 255, 0});
        }
        else if (goalColor == RGB({255, 255, 0}))
        {
            goalColor = RGB({0, 255, 0});
        }
        else if (goalColor == RGB({0, 255, 0}))
        {
            goalColor = RGB({0, 255, 255});
        }
        else if (goalColor == RGB({0, 255, 255}))
        {
            goalColor = RGB({0, 0, 255});
        }
        else if (goalColor == RGB({0, 0, 255}))
        {
            goalColor = RGB({255, 0, 255});
        }
        else if (goalColor == RGB({255, 0, 255}))
        {
            goalColor = RGB({255, 0, 0});
        }
    }
    else
    {
        return;
    }
}