#include "Setup.h"

Strip strip;
// publics
void Setup::run() {
    init();
    digitalWrite(LED_BUILTIN, LOW);
    WifiManager::connect("devlight", "HatJaNur5StundenGedauert");
    if (Storage::getIsSetup()) {
        restart();
    } else {
        first();
    }
    digitalWrite(LED_BUILTIN, HIGH);
}

// privates
void Setup::init() {
    strip = Strip();
    Serial.begin(115200);
    EEPROM.begin(64);
    pinMode(LED_BUILTIN, OUTPUT);
}

void Setup::first() {
    Serial.println("first");
    WiFiClient c;
    HttpClient client(c, "devlight.local", 80);
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
            }
        } else {
        }
    } else {
    }
}

void Setup::restart() {
    // if ip changes notify server
    Serial.println("restart");
    if (Utils::ipToString(Storage::getIp()) !=
        Utils::ipToString(WifiManager::getIp())) {
        Serial.println("ipchange");
        WiFiClient c;
        HttpClient client(c, "devlight.local", 80);
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
            } else {
            }
        } else {
        }
    }
}