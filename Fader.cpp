#include "Fader.h"

Fader::Fader()
{
}

Fader::~Fader()
{
}

void Fader::loop(int pTimeout)
{
    timeout = pTimeout;
    fading();
}

void Fader::fading()
{
    int rTo = 255;
    int gTo = 0;
    int bTo = 0;
    while (true)
    {
        fadeTo({255, 0, 0});   //rot
        fadeTo({255, 255, 0}); //gelb
        fadeTo({0, 255, 0});   //grün
        fadeTo({0, 255, 255}); //cyan
        fadeTo({0, 0, 255});   //blue
        fadeTo({255, 0, 255}); //magenta
    }
    /**
     * rot  100
     * gelb 110
     * grün 010
     * cyan 011
     * blue 001
     * mage 101
     */
}

void Fader::fadeTo(RGB colorTo)
{

    long prevMillis = 0;

    RGB color = Storage::getStripPattern().colors[0];
    int r = color.r;
    int g = color.g;
    int b = color.b;

    int rTo = colorTo.r;
    int gTo = colorTo.g;
    int bTo = colorTo.b;

    int rFac = rTo == r ? 0 : r < rTo ? 1
                                      : -1;
    int gFac = gTo == g ? 0 : g < gTo ? 1
                                      : -1;
    int bFac = bTo == b ? 0 : b < bTo ? 1
                                      : -1;

    while (r != rTo || g != gTo || b != bTo)
    {
        long currentMillis = millis();
        if (currentMillis - prevMillis > timeout)
        {
            int goalR = (r + rFac);
            int goalG = (g + gFac);
            int goalB = (b + bFac);

            r = ((goalR <= rTo && rFac == 1) || (goalR >= rTo && rFac == -1)) ? goalR : r;
            g = ((goalG <= gTo && gFac == 1) || (goalG >= gTo && gFac == -1)) ? goalG : g;
            b = ((goalB <= bTo && bFac == 1) || (goalB >= bTo && bFac == -1)) ? goalB : b;

            StripPattern pat;
            pat.colors[0] = {r, g, b};
            pat.pattern = 2;
            pat.timeout = timeout;
            strip.showPattern(pat);

            //-----debug----
            //Serial.print(r);
            //Serial.print(".");
            //Serial.print(g);
            //Serial.print(".");
            //Serial.println(b);
            //-----debug----
        }
    }
}