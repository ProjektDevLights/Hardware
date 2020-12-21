#include "WifiManager.h"

void WifiManager::connect(String ssid, String password)
{
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println(WiFi.localIP());
    Serial.println("\n");
}

IPAddress WifiManager::getIp()
{
    return WiFi.localIP();
}