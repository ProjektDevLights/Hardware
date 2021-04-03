#include "Setup.h"
#include "Control.h"

void setup()
{
    Setup::run();
}
Control control;
void loop()
{
    unsigned long t1 = millis();
    control.loop();
    unsigned long t2 = millis();
    if (t2 - t1 > 20)
    {
        Serial.print("loop time: ");
        Serial.println(t2 - t1);
    }
}