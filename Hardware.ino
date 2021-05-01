#include "Control.h"
#include "Setup.h"

Control control;

void setup() {
    Setup::run();
    control.setup();
}
void loop() { control.loop(); }