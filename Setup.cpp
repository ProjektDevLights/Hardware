#include "Setup.h"

Strip strip;
//publics
void Setup::run()
{
    init();
    digitalWrite(LED_BUILTIN, LOW);
    WifiManager::connect("devlight", "HatJaNur5StundenGedauert");
    if (Storage::getIsSetup())
    {
        restart();
    }
    else
    {
        first();
    }
    digitalWrite(LED_BUILTIN, HIGH);
}

//privates
void Setup::init()
{
    strip = Strip();
    Serial.begin(115200);
    Serial.println("");
    EEPROM.begin(512);
    pinMode(LED_BUILTIN, OUTPUT);
}

void Setup::first()
{
    Serial.println("Initializing first Setup");
    WiFiClient c;
    HttpClient client(c, "devlight.local", 80);
    int err = 0;
    String request = "/esp/setup?ip=";
    request += Utils::ipToString(WifiManager::getIp());
    err = client.get(request);
    if (err == 0)
    {
        err = client.responseStatusCode();
        if (err == 200)
        {
            err = client.skipResponseHeaders();
            if (err >= 0)
            {
                int bodyLength = client.contentLength();
                unsigned long timeoutStart = millis();
                String response = "";
                char c;
                while ((client.available() || client.connected()) && ((millis() - timeoutStart) < 30000))
                {
                    if (client.available())
                    {
                        c = client.read();
                        response += c;
                        bodyLength--;
                        timeoutStart = millis();
                    }
                    else
                    {
                        delay(500);
                    }
                }
                Storage::setId(response);
                Storage::setIp(WifiManager::getIp());
                Storage::setIsSetup(true);
                strip.setLength(150);
                StripPattern startup;
                startup.colors[0] = {0, 255, 106};
                startup.pattern = 1;
                Storage::setCount(0);
                strip.showPattern(startup);
                Storage::setStripPattern(startup);
                //Serial.println("succesfully registered");
            }
        }
        else
        {
            Serial.println("someone stole my ip 1");
        }
    }
    else
    {
        Serial.println("someone stole my ip 2");
    }
}

void Setup::restart()
{
    Serial.println("Initializing restart Setup");
    //if ip changes notify server
    if (Utils::ipToString(Storage::getIp()) != Utils::ipToString(WifiManager::getIp()))
    {
        Serial.println("ip changed");
        WiFiClient c;
        HttpClient client(c, "devlight.local", 80);
        int err = 0;
        //TODO save and read id
        String id = Storage::getId();
        String request = "/esp/update?ip=" + Utils::ipToString(WifiManager::getIp()) + "&id=" + id;
        err = client.get(request);
        if (err == 0)
        {
            err = client.responseStatusCode();
            if (err == 200)
            {
                Storage::setIp(WifiManager::getIp());
            }
            else
            {
                Serial.println("error updating ip 1");
            }
        }
        else
        {
            Serial.println("error updating ip 2");
        }
    }
}