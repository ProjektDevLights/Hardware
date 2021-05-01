#include "Utils.h"

String Utils::ipToString(IPAddress ip) {
    return String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." +
           String(ip[3]);
}

DynamicJsonDocument Utils::stringToJSON(String raw) {
    DynamicJsonDocument doc(2048);
    if (raw == "") {
        return doc;
    }
    deserializeJson(doc, raw);
    return doc;
}

RGB Utils::stringToRGB(String colorString) {
    RGB color;
    StringSplitter *splitter = new StringSplitter(colorString, '.', 3);
    color.r = splitter->getItemAtIndex(0).toInt();
    color.g = splitter->getItemAtIndex(1).toInt();
    color.b = splitter->getItemAtIndex(2).toInt();
    delete splitter;

    return color;
}

String Utils::patternIntToString(int patternNr) {
    switch (patternNr) {
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

StripPattern Utils::dataToStripPattern(String patternString,
                                       JsonArray colorArray, int timeout) {
    StripPattern pattern;
    if (patternString == "plain") {
        pattern.pattern = 1;
    }
    if (patternString == "fading") {
        pattern.pattern = 2;
    }
    if (patternString == "gradient") {
        pattern.pattern = 3;
    }
    if (patternString == "runner") {
        pattern.pattern = 4;
    }
    if (patternString == "rainbow") {
        pattern.pattern = 5;
    }
    for (int i = 0; i < 10; i++) {
                pattern.colors[i] = stringToRGB(colorArray[i]);
    }
    pattern.timeout = timeout;
    return pattern;
}

int Utils::RGBToHue(RGB color) {
    float r = color.r / 255.0f;
    float g = color.g / 255.0f;
    float b = color.b / 255.0f;

    float min = std::min(r, std::min(g, b));
    float max = std::max(r, std::max(g, b));

    float hue = 0;

    if (r == max) hue = (g - b) / (max - min);
    if (g == max) hue = 2 + (b - r) / (max - min);
    if (b == max) hue = 4 + (r - g) / (max - min);

    return hue * 60;
}

std::vector<RGB> Utils::generatePixelsOff(int length) {
    std::vector<RGB> arr;
    arr.resize(length);
    for (int i = 0; i < length; i++) {
        arr[i] = RGB({0, 0, 0});
    }
    return arr;
}
std::vector<RGB> Utils::generatePixelsColor(int length, RGB color) {
    std::vector<RGB> arr;
    arr.resize(length);
    std::fill(arr.begin(), arr.end(), color);
    return arr;
}

std::vector<RGB> Utils::generatePixels(int length, StripPattern pattern,
                                       int startIndex) {
    std::vector<RGB> arr;
    arr.resize(length);
    RGB firstColor = pattern.colors[0];
    RGB secondColor = pattern.colors[1];

    switch (pattern.pattern) {
        default:
        case 1:
        case 2:
        case 5:
            std::fill(arr.begin(), arr.end(), firstColor);
            break;
        case 3:
            for (int i = 0; i < length; i++) {
                arr[i] = interpolateColor(firstColor, secondColor, i, length);
            }
            break;
        case 4:
            std::fill(arr.begin(), arr.end(), RGB({0, 0, 0}));
            std::fill(arr.begin() + startIndex,
                      arr.begin() + startIndex + ceil(length / 15), firstColor);
            break;
    }
    return arr;
}

RGB Utils::interpolateColor(RGB first, RGB second, int step, int steps) {
    int r = interpolateValue(first.r, second.r, step, steps);
    int g = interpolateValue(first.g, second.g, step, steps);
    int b = interpolateValue(first.b, second.b, step, steps);
    return RGB({r, g, b});
}

int Utils::interpolateValue(int first, int second, int step, int steps) {
    return (first * (steps - step) + second * step) / steps;
}
