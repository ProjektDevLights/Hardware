#include "Utils.h"

String Utils::ipToString(IPAddress ip)
{
    return String(ip[0]) + "." + String(ip[1]) +
           "." + String(ip[2]) + "." + String(ip[3]);
}

StripPattern Utils::generatePattern(String patternString, JsonArray colorArray, int timeout)
{
    StripPattern pattern;
    String color = colorArray[0];
    if (patternString == "plain")
    {
        pattern.pattern = 1;
    }
    if (patternString == "fading")
    {
        pattern.pattern = 2;
    }
    if (patternString == "gradient")
    {
        pattern.pattern = 3;
    }
    if (patternString == "runner")
    {
        pattern.pattern = 4;
    }
    if (patternString == "rainbow")
    {
        pattern.pattern = 5;
    }
    for (int i = 0; i < 10; i++)
    {
        pattern.colors[i] = generateColor(colorArray[i]);
    }
    pattern.timeout = timeout;
    return pattern;
}

String Utils::generatePatternString(int patternNr)
{
    switch (patternNr)
    {
    case 1:
        return "plain";
    case 2:
        return "fading";
    case 3:
        return "gradient";
    case 4:
        return "runner";
    case 5:
        return "rainbow";
    default:
        return "error";
    }
}

RGB Utils::generateColor(String colorString)
{

    RGB color;
    StringSplitter *splitter = new StringSplitter(colorString, '.', 3);
    color.r = splitter->getItemAtIndex(0).toInt();
    color.g = splitter->getItemAtIndex(1).toInt();
    color.b = splitter->getItemAtIndex(2).toInt();
    delete splitter;

    return color;
}

DynamicJsonDocument Utils::stringToJSON(String raw)
{

    DynamicJsonDocument doc(2048);
    if (raw == "")
    {
        return doc;
    }
    deserializeJson(doc, raw);
    return doc;
}

int Utils::generateStep(int start, int end, int time, int delay)
{
    if (delay < 1)
    {
        delay = 1;
    }
    float floatStep = ((start - end) * delay) / time;
    return stepRound(floatStep);
}

int Utils::stepRound(float number)
{
    return number == 0 ? 0 : number < 0 ? floor(number)
                                        : ceil(number);
}

std::vector<RGB> Utils::offPixels(int length)
{
    std::vector<RGB> arr;
    arr.resize(length);
    for (int i = 0; i < length; i++)
    {
        arr[i] = RGB({0, 0, 0});
    }
    return arr;
}

std::vector<RGB> Utils::generatePixels(int length, StripPattern pattern, int startIndex)
{
    switch (pattern.pattern)
    {
    default:
    case 1:
    case 2:
    case 5:
        return generatePixelsSingle(length, pattern.colors[0]);
    case 3:
        return generatePixelsGradient(length, pattern.colors[0], pattern.colors[1]);
    case 4:
        return generatePixelsRunner(length, pattern.colors[0], startIndex);
    }
}

std::vector<RGB> Utils::generatePixelsSingle(int length, RGB color)
{

    std::vector<RGB> arr;
    arr.resize(length);
    for (int i = 0; i < length; i++)
    {
        arr[i] = color;
    }
    return arr;
}
std::vector<RGB> Utils::generatePixelsGradient(int length, RGB first, RGB second)
{

    std::vector<RGB> arr;
    arr.resize(length);

    float difR = (float)(first.r - second.r) / (float)length;
    float difG = (float)(first.g - second.g) / (float)length;
    float difB = (float)(first.b - second.b) / (float)length;

    int rStep = -Utils::stepRound(difR); // 255 - 0 /15
    int gStep = -Utils::stepRound(difG);
    int bStep = -Utils::stepRound(difB);

    int r = first.r;
    int g = first.g;
    int b = first.b;
    for (int i = 0; i < length; i++)
    {
        int goalR = r + rStep;
        int goalG = g + gStep;
        int goalB = b + bStep;

        r = goalR <= 255 && goalR >= 0 ? goalR : r;
        g = goalG <= 255 && goalG >= 0 ? goalG : g;
        b = goalB <= 255 && goalB >= 0 ? goalB : b;

        arr[i] = RGB({r, g, b});
    }
    return arr;
}
std::vector<RGB> Utils::generatePixelsRunner(int length, RGB color, int startIndex)
{
    std::vector<RGB> arr;
    arr.resize(length);
    for (int i = 0; i < length; i++)
    {
        arr[i] = RGB({0, 0, 0});
    }
    for (int i = startIndex; i < startIndex + ceil((float)length / 15.0f); i++)
    {
        arr[i] = color;
    }
    return arr;
}

RGB Utils::interpolate(RGB first, RGB second, int step, int steps)
{
    int r = (first.r * (steps - step) + second.r * step) / steps;
    int g = (first.g * (steps - step) + second.g * step) / steps;
    int b = (first.b * (steps - step) + second.b * step) / steps;
    return RGB({r, g, b});
}
