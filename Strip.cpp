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
    currentPattern = pattern;
    activePattern = pattern.pattern;
    switch (pattern.pattern)
    {
    case 1:
        if (noFade)
        {
            showColor(pattern.colors[0]);
        }
        else
        {
            fadeToColor(pattern.colors[0]);
        }

        Storage::setStripPattern(pattern);
        break;
    case 5:
    case 2:
        currentColor = Storage::getStripPattern().colors[0];
        Storage::setStripPattern(pattern);
        goalColor = RGB({255, 0, 0});
        break;
    case 3:
        Storage::setStripPattern(pattern);
        showGradient(pattern.colors[0], pattern.colors[1]);
        break;
    case 4:
        Storage::setStripPattern(pattern);
        currentColor = Storage::getStripPattern().colors[0];
        break;
    default:
        break;
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

void Strip::setBrightness(int b)
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
    Storage::setBrightness(b);
}

//private

void Strip::showColor(RGB color)
{
    pixels.clear();
    pixels.fill(generateColor(color.r, color.g, color.b), 0, length);
    delay(1);
    pixels.show();
}

void Strip::fadeToColor(RGB color)
{
    const int time = 40;
    RGB oldColor = Storage::getStripPattern().colors[0];

    int rStep = Utils::generateStep(oldColor.r, color.r, time);
    int gStep = Utils::generateStep(oldColor.g, color.g, time);
    int bStep = Utils::generateStep(oldColor.b, color.b, time);

    for (int i = 0; i < time; i++)
    {
        oldColor.r = oldColor.r - rStep;
        oldColor.g = oldColor.g - gStep;
        oldColor.b = oldColor.b - bStep;
        showColor(oldColor);
        yield();
    }
}

/**
 * should just be used for on/off
 */
void Strip::fadeToColor(RGB oldColor, RGB color)
{
    const int time = 40;
    int rStep = Utils::generateStep(oldColor.r, color.r, time);
    int gStep = Utils::generateStep(oldColor.g, color.g, time);
    int bStep = Utils::generateStep(oldColor.b, color.b, time);

    for (int i = 0; i < time; i++)
    {

        oldColor.r = oldColor.r - rStep;
        oldColor.g = oldColor.g - gStep;
        oldColor.b = oldColor.b - bStep;
        showColor(oldColor);
        yield();
    }
}

uint32_t Strip::generateColor(int r, int g, int b)
{
    return pixels.Color((r * brightness / 255), (g * brightness / 255), (b * brightness / 255));
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

        pixels.setPixelColor(i, generateColor(r, g, b));
    }
    pixels.show();
}

void Strip::showCustom(JsonArray colors)
{
    pixels.clear();
    Serial.println(colors.size());
    for (int i = 0; i < colors.size(); i++)
    {
        RGB color = Utils::generateColor(colors.getElement(i));
        Serial.println(color.toString());
        pixels.setPixelColor(i, generateColor(color.r, color.g, color.b));
    }
    pixels.show();
}

void Strip::fadeUpdate()
{

    unsigned long currentMillis = millis();

    if (currentMillis - lastUpdate > currentPattern.timeout)
    {
        lastUpdate = currentMillis;

        int r = currentColor.r;
        int g = currentColor.g;
        int b = currentColor.b;

        int rTo = goalColor.r;
        int gTo = goalColor.g;
        int bTo = goalColor.b;

        int rFac = rTo == r ? 0 : r < rTo ? 1
                                          : -1;
        int gFac = gTo == g ? 0 : g < gTo ? 1
                                          : -1;
        int bFac = bTo == b ? 0 : b < bTo ? 1
                                          : -1;

        int goalR = (r + rFac);
        int goalG = (g + gFac);
        int goalB = (b + bFac);

        r = ((goalR <= rTo && rFac == 1) || (goalR >= rTo && rFac == -1)) ? goalR : r;
        g = ((goalG <= gTo && gFac == 1) || (goalG >= gTo && gFac == -1)) ? goalG : g;
        b = ((goalB <= bTo && bFac == 1) || (goalB >= bTo && bFac == -1)) ? goalB : b;
        showColor({r, g, b});
        currentColor = RGB({r, g, b});
        currentPattern.colors[0] = RGB({r, g, b});

        if (currentColor == goalColor)
        {
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
        const int leds = round(count / 20);
        pixels.clear();
        for (int i = currentStartLed; i < currentStartLed + leds; i++)
        {
            pixels.setPixelColor(i, generateColor(currentColor.r, currentColor.g, currentColor.b));
        }
        pixels.show();
        if (currrentDirectionRight)
        {
            currentStartLed++;
            if (currentStartLed > count)
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

void Strip::rainbowUpdate()
{

    unsigned long currentMillis = millis();
    if (currentMillis - lastUpdate > currentPattern.timeout)
    {
        lastUpdate = currentMillis;
        pixels.fill(generateColor(currentColor.r, currentColor.g, currentColor.b), 0, length);
        pixels.show();
        currentColor = goalColor;
        setNewGoal();
    }
}