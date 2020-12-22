#include "Strip.h"

#ifdef __AVR__
#include <avr/power.h>
#endif
#define PIN 5 //D1
#define PIXELFORMAT NEO_GRB + NEO_KHZ800
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
clock_prescale_set(clock_div_1);
#endif

Adafruit_NeoPixel pixels(150, PIN, PIXELFORMAT);
Strip::Strip()
{
    pixels.begin();
}

void Strip::showPattern(StripPattern pattern)
{
    Storage::setStripPattern(pattern);
    /**
     * Patterns:
     * plain: 1
     * fading: 2
     * 
     *
     */
    if (pattern.pattern == 1 || pattern.pattern == 2)
    {
        showColor(pattern.colors[0]);
    }
    else if (pattern.pattern == 3)
    {
        showGradient(pattern.colors[0], pattern.colors[1]);
    }
    else
    {
        Serial.println("Wrong pattern");
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
    delay(10);
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

    Serial.println("difs");
    Serial.println(difR);
    Serial.println(difG);
    Serial.println(difB);

    Serial.println(Utils::stepRound(difR));
    Serial.println(Utils::stepRound(difG));
    Serial.println(Utils::stepRound(difB));

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
        Serial.print(r);
        Serial.print(".");
        Serial.print(g);
        Serial.print(".");
        Serial.println(b);
        //-----debug----

        pixels.setPixelColor(i, r, g, b);
    }
    pixels.show();
}
