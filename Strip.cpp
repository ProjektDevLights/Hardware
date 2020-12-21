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
    if (pattern.pattern == 1)
    {
        showColor(pattern.colors[0]);
    }
    else
    {
        Serial.println("Wrong pattern");
    }
}

bool Strip::setLength(int length, lengthCallback callback)
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