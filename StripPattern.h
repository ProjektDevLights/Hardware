#include "RGB.h"

#ifndef StripPattern_h
#define StripPattern_h
struct StripPattern
{
    RGB colors[10];
    /**
     * Patterns:
     * plain: 1
     * fading: 2
     * gradient: 3
     * runner: 4
     * rainbow: 5
     *
     */
    int pattern;
    int timeout;
    String toString()
    {
        String string = String("");
        for (int i = 0; i < 10; i++)
        {
            string.concat("Color ");
            string.concat(i + 1);
            string.concat(": ");
            string.concat(colors[i].toString());
            string.concat("\n");
        }
        string.concat("Pattern: ");
        string.concat(pattern);
        string.concat("\n");

        string.concat("Timeout: ");
        string.concat(timeout);
        return string;
    };
};
#endif