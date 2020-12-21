#include "Control.h"

Control::Control() : server(2389)
{
    server.begin();
    server.setNoDelay(true);
}

void Control::loop()
{
    DynamicJsonDocument data = stringToJSON(readData());
    String command = data["command"];
    if (!data.isNull())
    {
        if (command == "count")
        {
            int count = (int)data["data"];
            strip.setLength(count, []() {
                strip.showPattern(Storage::getStripPattern());
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
            strip.clear();
            ESP.restart();
        }
        if (command == "reset")
        {
            Storage::clear();
            strip.clear();
            ESP.restart();
        }
    }
    delay(1000);
}

String Control::readData()
{
    //read data from server if available ---------------------------------
    WiFiClient client = server.available();
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
    /*     digitalWrite(LED_BUILTIN, LOW);
    delay(50);
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println(readString);
    Serial.println("Ending connection");
    */
    client.println("Ending connection");
    client.stop();

    return readString;
}

DynamicJsonDocument Control::stringToJSON(String raw)
{
    DynamicJsonDocument doc(2048);
    deserializeJson(doc, raw);
    return doc;
}
