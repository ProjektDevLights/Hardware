#include "Control.h"

Control::Control()
{
}

void Control::setup()
{
    client.connect("devlight.local", 2389);

    if (Storage::getIsCustom())
    {
        sendPattern();
    }

    strip.setLength(Storage::getCount());
    strip.setBrightness(Storage::getBrightness(), true);

    StripPattern pattern = Storage::getStripPattern();
    strip.showPattern(pattern);
}

void Control::loop()
{

    strip.update();

    DynamicJsonDocument data = Utils::stringToJSON(readData());
    String command = data["command"];
    if (!data.isNull())
    {
        if (command == "count")
        {
            int count = (int)data["data"];
            strip.setLength(count);
        }
        if (command == "leds")
        {
            StripPattern pattern = Utils::generatePattern(data["data"]["pattern"], data["data"]["colors"], data["data"]["timeout"]);
            bool noFade = (bool)data["data"]["noFade"];
            // const bool noFade = noFadeJson.is<bool>();
            Storage::setStripPattern(pattern);
            strip.showPattern(pattern, noFade);
        }
        if (command == "off")
        {
            Storage::setIsOn(false);
            strip.stopRunning();
            strip.showOff(true);
        }
        if (command == "on")
        {
            Storage::setIsOn(true);

            StripPattern pattern = Storage::getStripPattern();
            strip.showPattern(pattern);
        }
        if (command == "brightness")
        {
            strip.setBrightness((int)data["data"]);
            Storage::setBrightness((int)data["data"]);
        }
        if (command == "restart")
        {
            client.write("shutdown\n");
            client.stop();
            strip.showOff(true);
            ESP.restart();
        }
        if (command == "serverRestart")
        {
            delay(5000);
            client.write("shutdown\n");
            client.stop();
            ESP.restart();
        }
        if (command == "reset")
        {
            Storage::clear();
            strip.showOff(true);
            client.write("shutdown\n");
            client.stop();
            ESP.restart();
        }
        if (command == "blink")
        {
            RGB color = Utils::generateColor(data["data"]["color"]);
            strip.showColor(color, true);
            delay(int(data["data"]["time"]));
            strip.showCurrentPattern();
        }
        if (command == "logStorage")
        {
            Storage::print();
        }
        if (command == "custom")
        {
            strip.stopRunning();
            Storage::setIsCustom(true);
            JsonArray array = data["data"];
            strip.showCustom(array);
            serializeJsonPretty(array, Serial);
        }
        else if (command != "logStorage")
        {
            Storage::setIsCustom(false);
            sendPattern();
        }
    }
}

String Control::readData()
{
    //read data from server if available ---------------------------------
    if (!client)
    {
        return "";
    }

    if (!client.connected())
    {

        return "";
    }

    String readString = "";

    while (client.available())
    {
        char c = client.read();
        if (c == '\n')
        {
            break;
        }
        readString += c;
        yield();
    }
    if (readString != "")
    {
        Serial.println(readString);
    }

    //  indicate incomming data (debug);
    /* digitalWrite(LED_BUILTIN, LOW);
    delay(50);
    digitalWrite(LED_BUILTIN, HIGH); */
    return readString;
}

void Control::sendPattern()
{
    DynamicJsonDocument jsonDoc(512);
    DynamicJsonDocument arrayDoc(512);
    JsonArray colors = arrayDoc.to<JsonArray>();
    StripPattern pattern = Storage::getStripPattern();
    jsonDoc["pattern"] = Utils::generatePatternString(pattern.pattern);
    if (pattern.timeout > 0)
    {
        jsonDoc["timeout"] = pattern.timeout;
    }
    for (RGB color : pattern.colors)
    {
        colors.add(color.toString());
    }
    jsonDoc["colors"] = colors;
    String jsonString;
    serializeJson(jsonDoc, jsonString);
    client.print(jsonString);
}
