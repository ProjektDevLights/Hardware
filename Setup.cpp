#include "Setup.h"

Strip strip;
// publics
bool Setup::run() {
    init();
    digitalWrite(LED_BUILTIN, LOW);
    WifiManager::connect("devlight", "HatJaNur5StundenGedauert");

    bool success = Storage::getIsSetup() ? restart() : first();
    if (!success) return false;
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
        ESP.restart();
    });
    ArduinoOTA.begin();
    return true;
}

// privates
void Setup::init() {
    strip = Strip();
    Serial.begin(115200);
    EEPROM.begin(2048);
    pinMode(LED_BUILTIN, OUTPUT);
}

bool Setup::first() {
    WiFiClient c;
    HttpClient client(c, "192.168.0.236", 80);
    int err = 0;
    String request = "/esp/setup?ip=";
    request += Utils::ipToString(WifiManager::getIp());
    err = client.get(request);
    if (err == 0) {
        err = client.responseStatusCode();
        if (err == 200) {
            err = client.skipResponseHeaders();
            if (err >= 0) {
                int bodyLength = client.contentLength();
                unsigned long timeoutStart = millis();
                String response = "";
                char c;
                while ((client.available() || client.connected()) &&
                       ((millis() - timeoutStart) < 30000)) {
                    if (client.available()) {
                        c = client.read();
                        response += c;
                        bodyLength--;
                        timeoutStart = millis();
                    } else {
                        delay(500);
                    }
                }
                Storage::setId(response);
                Storage::setIp(WifiManager::getIp());
                Storage::setIsSetup(true);
                Storage::setCount(150);
                Storage::setBrightness(255);
                Storage::setIsOn(true);
                StripPattern startup;
                startup.colors[0] = {29, 233, 182};
                startup.pattern = 1;
                Storage::setStripPattern(startup);
                return true;
            }
        } else {
            return false;
        }
    } else {
        return false;
    }
    return false;
}

bool Setup::restart() {
    // if ip changes notify server
    Serial.println("restart");
    if (Utils::ipToString(Storage::getIp()) !=
        Utils::ipToString(WifiManager::getIp())) {
        Serial.println("ipchange");
        WiFiClient c;
        HttpClient client(c, "192.168.0.236", 80);
        int err = 0;
        // TODO save and read id
        String id = Storage::getId();
        String request =
            "/esp/update?ip=" + Utils::ipToString(WifiManager::getIp()) +
            "&id=" + id;
        err = client.get(request);
        if (err == 0) {
            err = client.responseStatusCode();
            if (err == 200) {
                Storage::setIp(WifiManager::getIp());
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
        return false;
    }
    return true;
}
