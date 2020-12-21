#include "Control.h"

Control::Control() : server(2389)
{
    server.begin();
    server.setNoDelay(true);
}

void Control::loop()
{
    DynamicJsonDocument data = Utils::stringToJSON(readData());
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
        if (command == "fade")
        {
            fading(100);
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
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println(readString);
    Serial.println("Ending connection");

    client.println("Ending connection");
    client.stop();

    return readString;
}

void Control::fading(int timeout)
{
    int rTo = 255;
    int gTo = 0;
    int bTo = 0;
    while (true)
    {
        fadeTo({255, 0, 0}, timeout);
        fadeTo({255, 255, 0}, timeout);
        fadeTo({0, 255, 0}, timeout);
        fadeTo({0, 255, 255}, timeout);
        fadeTo({0, 0, 255}, timeout);
        fadeTo({255, 0, 255}, timeout);
    }
    /**
     * rot  100
     * gelb 110
     * grün 010
     * cyan 011
     * blue 001
     * mage 101
     */
}

void Control::fadeTo(RGB colorTo, int timeout)
{

    RGB color = Storage::getStripPattern().colors[0];
    int r = color.r;
    int g = color.g;
    int b = color.b;

    int rTo = colorTo.r;
    int gTo = colorTo.g;
    int bTo = colorTo.b;

    int rFac = r == rTo ? 0 : r < rTo ? 1 : -1;
    int gFac = g == gTo ? 0 : g < gTo ? 1 : -1;
    int bFac = b == bTo ? 0 : b < bTo ? 1 : -1;

    Serial.println("---------------");
    Serial.println(r);
    Serial.println(g);
    Serial.println(b);
    Serial.println("---------------");

    Serial.println(rTo);
    Serial.println(gTo);
    Serial.println(bTo);
    Serial.println("---------------");

    Serial.println(rFac);
    Serial.println(gFac);
    Serial.println(bFac);

    while (r != rTo || g != gTo || b != bTo)
    {

        int goalR = (r + rFac);
        int goalG = (g + gFac);
        int goalB = (b + bFac);
        //rot
        //Serial.println(goalR > 0 && goalR < rTo ? goalR : goalR < 255 && goalR > rTo ? rTo : r);
        if (goalR <= rTo && goalR >= 0)
        {
            r = goalR;
        }
        else if (goalR >= rTo && goalR <= 255)
        {
            r = goalR;
        }

        //grün
        if (goalG <= gTo && goalG >= 0)
        {
            g = goalG;
        }
        else if (goalG >= gTo && goalG <= 255)
        {
            g = goalG;
        }

        //blau
        if (goalB <= bTo && goalB >= 0)
        {
            b = goalB;
        }
        else if (goalB >= bTo && goalB <= 255)
        {
            b = goalB;
        }

        //        g = goalG > 0 && goalG < gTo ? goalG : goalG < 255 && goalG > gTo ? gTo : g;
        //        b = goalB > 0 && goalB < bTo ? goalB : goalB < 255 && goalB > bTo ? bTo : b;
        strip.showColor({r, g, b});
        Serial.print(r);
        Serial.print(".");
        Serial.print(g);
        Serial.print(".");
        Serial.println(b);
        delay(timeout);
    }
    Serial.println("--end--");
    StripPattern pat;
    pat.colors[0] = {r, g, b};
    pat.pattern = 1;
    Storage::setStripPattern(pat);
}
