#include "Setup.h"
#include "Control.h"

void setup()
{
    Setup::run();
}
Control control;
void loop()
{
    control.loop();
    delay(100);
}