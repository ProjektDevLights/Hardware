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
RGB goalColor;
unsigned long lastUpdate;

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

    default:
        break;
    }
}

void Strip::showPattern(StripPattern pattern)
{
    /**
     * Patterns:
     * plain: 1
     * fading: 2
     * gradient: 3
     *
     */
    Serial.println(pattern.timeout);
    currentPattern = pattern;
    activePattern = pattern.pattern;
    if (pattern.pattern == 1)
    {
        Storage::setStripPattern(pattern);
        showColor(pattern.colors[0]);
    }
    else if (pattern.pattern == 2)
    {
        currentColor = Storage::getStripPattern().colors[0];
        Storage::setStripPattern(pattern);
        goalColor = RGB({255, 0, 0});
    }
    else if (pattern.pattern == 3)
    {
        Storage::setStripPattern(pattern);
        showGradient(pattern.colors[0], pattern.colors[1]);
    }
    else
    {
    }
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

bool Strip::setLength(int length)
{
    if (length >= 0)
    {
        Storage::setCount(length);
        pixels.clear();
        pixels.show();
        delay(10);
        pixels.updateLength(length);
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

void Strip::setBrightness(int brightness)
{
    pixels.setBrightness(brightness);
    pixels.show();
}

//private

void Strip::showColor(RGB color)
{
    pixels.clear();
    pixels.fill(pixels.Color(color.r, color.g, color.b), 0, Storage::getCount());
    delay(1);
    pixels.show();
}

void Strip::showGradient(RGB first, RGB second)
{
    int count = Storage::getCount();

    float difR = (float)(first.r - second.r) / (float)count;
    float difG = (float)(first.g - second.g) / (float)count;
    float difB = (float)(first.b - second.b) / (float)count;

    int rStep = -Utils::stepRound(difR); // 255 - 0 /15
    int gStep = -Utils::stepRound(difG);
    int bStep = -Utils::stepRound(difB);

    int r = first.r;
    int g = first.g;
    int b = first.b;
    for (int i = 0; i < count; i++)
    {
        int goalR = r + rStep;
        int goalG = g + gStep;
        int goalB = b + bStep;

        r = goalR <= 255 && goalR >= 0 ? goalR : r;
        g = goalG <= 255 && goalG >= 0 ? goalG : g;
        b = goalB <= 255 && goalB >= 0 ? goalB : b;

        //-----debug----
        //Serial.print(r);
        //Serial.print(".");
        //Serial.print(g);
        //Serial.print(".");
        //Serial.println(b);
        //-----debug----

        pixels.setPixelColor(i, r, g, b);
    }
    pixels.show();
}

void Strip::fadeUpdate()
{

    unsigned long currentMillis = millis();

    if (currentMillis - lastUpdate > currentPattern.timeout)
    {
        Serial.print("current: ");
        Serial.println(currentMillis);
        Serial.print("last: ");
        Serial.println(lastUpdate);
        Serial.println(currentMillis - lastUpdate);
        Serial.println("fade");
        setNewGoal();
        lastUpdate = currentMillis;

        int r = currentColor.r;
        int g = currentColor.g;
        int b = currentColor.b;

        int rTo = goalColor.r;
        int gTo = goalColor.g;
        int bTo = goalColor.b;

        int rFac = rTo == r ? 0 : r < rTo ? 1 : -1;
        int gFac = gTo == g ? 0 : g < gTo ? 1 : -1;
        int bFac = bTo == b ? 0 : b < bTo ? 1 : -1;

        int goalR = (r + rFac);
        int goalG = (g + gFac);
        int goalB = (b + bFac);

        r = ((goalR <= rTo && rFac == 1) || (goalR >= rTo && rFac == -1)) ? goalR : r;
        g = ((goalG <= gTo && gFac == 1) || (goalG >= gTo && gFac == -1)) ? goalG : g;
        b = ((goalB <= bTo && bFac == 1) || (goalB >= bTo && bFac == -1)) ? goalB : b;
        showColor({r, g, b});
        currentColor = RGB({r, g, b});
        currentPattern.colors[0] = RGB({r, g, b});
        Storage::setStripPattern(currentPattern);
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
}