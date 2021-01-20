#include "Control.h"

Control::Control()
{
    // Serial.println();
}

bool first = true;

void Control::loop()
{
    if (first)
    {
        Serial.println(client.connect("devlight.local", 2389));
        first = false;
        initStrip();
    }

    DynamicJsonDocument data = Utils::stringToJSON(readData());
    String command = data["command"];
    if (!data.isNull())
    {
        if (command == "count")
        {
            int count = (int)data["data"];
            strip.setLength(count, [this]() {
                initStrip();
            });
        }
        if (command == "leds")
        {
            StripPattern pattern = Utils::generatePattern(data["data"]["pattern"], data["data"]["colors"]);
            strip.showPattern(pattern);
        }
        if (command == "off")
        {
            strip.clear();
        }
        if (command == "on")
        {
            strip.showPattern(Storage::getStripPattern());
        }
        if (command == "brightness")
        {
            strip.setBrightness((int)data["data"]);
        }
        if (command == "restart")
        {
            client.write("shutdown\n");
            client.stop();
            strip.clear();
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
            strip.clear();
            client.write("shutdown\n");
            client.stop();
            ESP.restart();
        }
        if (command == "blink")
        {
            RGB color = Utils::generateColor(data["data"]["color"]);
            strip.showColor(color);
            delay(int(data["data"]["time"]));
            initStrip();
        }
        if (command == "blink")
        {
            RGB color = Utils::generateColor(data["data"]["color"]);
            Serial.println(color.r);
            Serial.println(color.g);
            Serial.println(color.b);

            strip.showColor(color);
            delay(int(data["data"]["time"]));
            initStrip();
        }
        if (command == "rainbow")
        {
            while (true)
            {
                strip.showColor({255, 0, 0});
                delay(1000);
                strip.showColor({255, 255, 0});
                delay(1000);
                strip.showColor({0, 255, 0});
                delay(1000);
                strip.showColor({0, 255, 255});
                delay(1000);
                strip.showColor({0, 0, 255});
                delay(1000);
                strip.showColor({255, 0, 255});
                delay(1000);
            }
        }
        if (command == "fade")
        {
            // BIG TODO still endless loop (esp is single threaded)

            fader.loop((int)data["data"]);
        }
    }
    delay(1000);
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
    while (client.connected())
    {
        if (client.available())
        {
            char c = client.read();
            if (c == '\n')
            {
                break;
            }
            readString += c;
        }
    }

    //  indicate incomming data (debug);
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println(readString);
    Serial.println("Ending connection");
    client.println("Ending connection");
    return readString;
}
void Control::initStrip()
{
    Serial.println("init strip");
    StripPattern pattern = Storage::getStripPattern();
    if (pattern.pattern == 1 || pattern.pattern == 3)
    {
        Serial.println("plain or gradient pattern");
        strip.showPattern(pattern);
    }
    if (pattern.pattern == 2)
    {
        Serial.println("fade pattern");
    }
}
