#include "WifiManager.h"

void WifiManager::connect(String ssid, String password) {
    WiFi.begin(ssid, password);
    WiFi.softAPdisconnect(true);
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
    }
    Serial.println("\n");
}

IPAddress WifiManager::getIp() { return WiFi.localIP(); }