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