#include "Strip.h"

#ifdef __AVR__
#include <avr/power.h>
#endif
#define PIN 5  // D1
#define PIXELFORMAT NEO_GRB + NEO_KHZ800
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
clock_prescale_set(clock_div_1);
#endif

Adafruit_NeoPixel pixels(1000, PIN, PIXELFORMAT);

// public

Strip::Strip() { pixels.begin(); }

void Strip::showCurrentPattern(bool noFade) { showPattern(curPattern, noFade); }

void Strip::showPattern(StripPattern pattern, bool noFade) {
    /**
     * Patterns:
     * plain: 1
     * fading: 2
     * gradient: 3
     * runner: 4
     * rainbow: 5
     */

    // evtl wegnehmen wenn noFade
    std::vector<RGB> oldColors = readStrip();
    std::vector<RGB> newColors =
        Utils::generatePixels(length, pattern, getFirstRunLed());
    noFade ? showPixelArray(newColors) : fadeToPixelArray(oldColors, newColors);
    curPattern = pattern;
    activePattern = pattern.pattern;
    curFadeIndex = 0;

    hue = Utils::RGBToHue(pattern.colors[0]);
}

void Strip::showColor(RGB color, bool noFade) {
    if (noFade) {
        pixels.clear();
        pixels.fill(RGBToPixelColor(color), 0, length);
        pixels.show();
    } else {
        fadeToPixelArray(readStrip(),
                         Utils::generatePixelsColor(length, color));
    }
}

void Strip::showCustom(JsonArray colors) {
    pixels.clear();
    Serial.println(colors.size());
    for (int i = 0; i < colors.size(); i++) {
        RGB color = Utils::stringToRGB(colors.getElement(i));
        Serial.println(color.toString());
        pixels.setPixelColor(i, RGBToPixelColor(color));
    }
    pixels.show();
}

void Strip::showOff(bool noFade) {
    if (noFade) {
        pixels.clear();
        pixels.show();
        return;
    } else {
        std::vector<RGB> before = readStrip();
        std::vector<RGB> after = Utils::generatePixelsOff(length);
        fadeToPixelArray(before, after);
        pixels.clear();
        pixels.show();
    }
}

bool Strip::setLength(int pLength) {
    if (length >= 0) {
        Storage::setCount(pLength);
        pixels.clear();
        pixels.show();
        pixels.updateLength(pLength);
        length = pLength;
        showCurrentPattern(true);
        return true;
    }
    return false;
}

bool Strip::setBrightness(int b, bool silent) {
    if (!(b >= 0)) return false;
    if (silent) {
        brightness = b;
    } else {
        int runs = 30;
        int bF = brightness;
        for (int i = 0; i < runs; i++) {
            brightness = Utils::interpolateValue(bF, b, i, runs);
            showCurrentPattern(true);
            yield();
        }
        brightness = b;
    }
    Storage::setBrightness(b);
    return true;
}

void Strip::update() {
    switch (activePattern) {
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

void Strip::stopRunning() { activePattern = -1; }

// private
void Strip::showPixelArray(std::vector<RGB> colors) {
    for (int i = 0; i < length; i++) {
        pixels.setPixelColor(i, RGBToPixelColor(colors[i]));
        yield();
    }
    pixels.show();
}

void Strip::fadeToPixelArray(std::vector<RGB> from, std::vector<RGB> to) {
    std::vector<RGB> oldColors;

    int runs = 40;

    for (int i = 0; i < runs; i++) {
        for (int j = 0; j < length; j++) {
            pixels.setPixelColor(j, RGBToPixelColor(Utils::interpolateColor(
                                        from[j], to[j], i, runs)));
        }
        yield();
        pixels.show();
    }
    for (int i = 0; i < length; i++) {
        pixels.setPixelColor(i, RGBToPixelColor(to[i]));
    }
    pixels.show();
}

void Strip::fadeUpdate() {
    unsigned long currentMillis = millis();
    if (currentMillis - lastUpdate > curPattern.timeout) {
        RGB first = pixelColorToRGB(pixels.ColorHSV(hue * 182));
        RGB second =
            pixelColorToRGB(pixels.ColorHSV(((hue + 120) % 360) * 182));
        RGB curCol = Utils::interpolateColor(first, second, curFadeIndex, 255);
        showColor(curCol, true);
        curFadeIndex++;
        lastUpdate = currentMillis;

        curPattern.colors[0] = curCol;
        if (curCol == second) {
            curFadeIndex = 0;
            hue = (hue + 120) % 360;
            curPattern.colors[0] = curCol;
            Storage::setStripPattern(curPattern);
        }
    }
}

void Strip::rainbowUpdate() {
    unsigned long currentMillis = millis();
    if (currentMillis - lastUpdate > curPattern.timeout) {
        lastUpdate = currentMillis;
        pixels.fill(pixels.ColorHSV(hue * 182), 0, length);
        pixels.show();
        hue = (hue + 60) % 360;
    }
}

void Strip::runnerUpdate() {
    unsigned long currentMillis = millis();
    if (currentMillis - lastUpdate > curPattern.timeout) {
        lastUpdate = currentMillis;

        const int leds = ceil((float)length / 15.0f);
        int firstLed = getFirstRunLed();

        if (curRunIndex == length - leds) {
            curRunIndex = 0;
            curDirectionForward = !curDirectionForward;
        }
        curRunIndex++;

        pixels.clear();
        for (int i = firstLed; i < firstLed + leds; i++) {
            pixels.setPixelColor(i, RGBToPixelColor(curPattern.colors[0]));
        }
        pixels.show();
    }
}

std::vector<RGB> Strip::readStrip() {
    std::vector<RGB> arr;
    arr.resize(length);
    for (int i = 0; i < length; i++) {
        arr[i] = pixelColorToRGB(pixels.getPixelColor(i));
    }
    return arr;
}

int Strip::getFirstRunLed() {
    int leds = ceil((float)length / 15.0f);
    bool f = curDirectionForward;
    int from = f ? 0 : length - leds;
    int to = f ? length - leds : 0;

    return Utils::interpolateValue(from, to, curRunIndex, length - leds);
}

uint32_t Strip::RGBToPixelColor(RGB color) {
    return pixels.Color((color.r * brightness / 255),
                        (color.g * brightness / 255),
                        (color.b * brightness / 255));
}

RGB Strip::pixelColorToRGB(uint32_t color) {
    uint8_t r = color >> 16;
    uint8_t g = color >> 8;
    uint8_t b = color;

    return RGB({r, g, b});
}