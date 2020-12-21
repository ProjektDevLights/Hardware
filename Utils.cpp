#include "Utils.h"

String Utils::ipToString(IPAddress ip)
{
    return String(ip[0]) + "." + String(ip[1]) +
           "." + String(ip[2]) + "." + String(ip[3]);
}

StripPattern Utils::generatePattern(String patternString, JsonArray colorArray)
{
    StripPattern pattern;
    String color = colorArray[0];
    if (patternString == "plain")
    {
        pattern.pattern = 1;
    }
    for (int i = 0; i < 10; i++)
    {
        StringSplitter *splitter = new StringSplitter(colorArray[i], '.', 3);
        pattern.colors[i].r = splitter->getItemAtIndex(0).toInt();
        pattern.colors[i].g = splitter->getItemAtIndex(1).toInt();
        pattern.colors[i].b = splitter->getItemAtIndex(2).toInt();
    }
    return pattern;
}

DynamicJsonDocument Utils::stringToJSON(String raw)
{
    DynamicJsonDocument doc(2048);
    deserializeJson(doc, raw);
    return doc;
}