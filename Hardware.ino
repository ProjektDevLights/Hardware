#include "Control.h"
#include "Setup.h"

Control control;

void setup() {
    if (Setup::run()) {
        control.setup();
        digitalWrite(LED_BUILTIN, HIGH);
    } else {
        Serial.println("Error setting up!");
        Utils::blink(20, 200, 400);
    }
}
void loop() { control.loop(); }