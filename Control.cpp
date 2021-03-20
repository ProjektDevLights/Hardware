#include "Control.h"

Control::Control()
{
}

bool first = true;

void Control::loop()
{
    if (first)
    {
        client.connect("192.168.188.62", 2389);
        first = false;
        initStrip();
    }

    strip.update();

    DynamicJsonDocument data = Utils::stringToJSON(readData());
    String command = data["command"];
    yield();
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
            StripPattern pattern = Utils::generatePattern(data["data"]["pattern"], data["data"]["colors"], data["data"]["timeout"]);
            strip.showPattern(pattern);
        }
        if (command == "off")
        {
            Storage::setIsOn(false);
            strip.stopRunning();
            strip.fadeToColor(Storage::getStripPattern().colors[0], RGB({0, 0, 0}));
            strip.clear();
        }
        if (command == "on")
        {
            Storage::setIsOn(true);
            strip.fadeToColor(RGB({0, 0, 0}), Storage::getStripPattern().colors[0]);
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
        /*         if (command == "rainbow")
        {
            while (true)
            {
                // millis
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
        } */
    }

    /*     DynamicJsonDocument *ptrData = &data;
    String *ptrCommand = &command;
    delete ptrData;
    delete ptrCommand; */
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
        client.println("Ending connection\n");
    }

    //  indicate incomming data (debug);
    /* digitalWrite(LED_BUILTIN, LOW);
    delay(50);
    digitalWrite(LED_BUILTIN, HIGH); */
    return readString;
}
void Control::initStrip()
{

    strip.setLength(Storage::getCount());
    strip.setBrightness(Storage::getBrightness());
    if (Storage::getIsOn())
    {
        StripPattern pattern = Storage::getStripPattern();
        strip.showPattern(pattern);
    }
}
