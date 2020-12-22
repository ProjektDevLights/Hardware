#ifndef Fader_h
#define Fader_h

#include "RGB.h"
#include "StripPattern.h"
#include "Storage.h"
#include "GlobalStrip.h"

class Fader
{
private:
    int timeout;
    void fading();
    void fadeTo(RGB color);

public:
    Fader();
    ~Fader();
    void loop(int timeout);
};

#endif