#include "Control.h"

void Control::setup() {
    client.connect("devlight.local", 2389);

    if (Storage::getIsCustom()) {
        sendPattern();
    }

    strip.setLength(Storage::getCount());
    strip.setBrightness(Storage::getBrightness(), true);

    if (Storage::getIsOn()) {
        StripPattern pattern = Storage::getStripPattern();
        strip.showPattern(pattern);
    }
}

void Control::loop() {
    ArduinoOTA.handle();

    strip.update();

    DynamicJsonDocument data = Utils::stringToJSON(readData());
    String command = data["command"];
    if (!data.isNull()) {
        if (command == "leds") {
            StripPattern pattern = Utils::dataToStripPattern(
                data["data"]["pattern"], data["data"]["colors"],
                data["data"]["timeout"]);
            bool noFade = (bool)data["data"]["noFade"];
            strip.showPattern(pattern, noFade);
            Storage::setStripPattern(pattern);
        }
        if (command == "on") {
            Storage::setIsOn(true);
            strip.showCurrentPattern();
        }
        if (command == "off") {
            Storage::setIsOn(false);
            strip.stopRunning();
            strip.showOff();
        }
        if (command == "brightness") {
            strip.setBrightness((int)data["data"]);
            Storage::setBrightness((int)data["data"]);
        }
        if (command == "blink") {
            RGB color = Utils::stringToRGB(data["data"]["color"]);
            strip.showColor(color, true);
            delay(int(data["data"]["time"]));
            strip.showCurrentPattern();
        }
        if (command == "custom") {
            strip.stopRunning();
            JsonArray array = data["data"];
            strip.showCustom(array);
            Storage::setIsCustom(true);
            isCustom = true;
        } else if (command != "logStorage") {
            if (isCustom) {
                Storage::setIsCustom(false);
                sendPattern();
                isCustom = false;
            }
        }
        if (command == "count") {
            int count = (int)data["data"];
            strip.setLength(count);
        }
        if (command == "serverRestart") {
            delay(5000);
            client.write("shutdown\n");
            client.stop();
            ESP.restart();
        }
        if (command == "restart") {
            client.write("shutdown\n");
            client.stop();
            strip.showOff(true);
            ESP.restart();
        }
        if (command == "reset") {
            Storage::clear();
            strip.showOff(true);
            client.write("shutdown\n");
            client.stop();
            ESP.restart();
        }

        if (command == "logStorage") {
            Storage::print();
        }
    }
}

String Control::readData() {
    // read data from server if available
    if (!client) {
        return "";
    }

    if (!client.connected()) {
        return "";
    }

    String readString = "";

    while (client.available()) {
        char c = client.read();
        if (c == '\n') {
            break;
        }
        readString += c;
        yield();
    }
    return readString;
}

void Control::sendPattern() {
    DynamicJsonDocument jsonDoc(512);
    DynamicJsonDocument arrayDoc(512);
    JsonArray colors = arrayDoc.to<JsonArray>();
    StripPattern pattern = Storage::getStripPattern();
    jsonDoc["pattern"] = Utils::patternIntToString(pattern.pattern);
    if (pattern.timeout > 0) {
        jsonDoc["timeout"] = pattern.timeout;
    }
    for (RGB color : pattern.colors) {
        colors.add(color.toString());
    }
    jsonDoc["colors"] = colors;
    String jsonString;
    serializeJson(jsonDoc, jsonString);
    client.print(jsonString);
}
