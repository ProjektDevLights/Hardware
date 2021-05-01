#include "Arduino.h"

#ifndef RGB_h
#define RGB_h

struct RGB {
    int r;
    int g;
    int b;
    bool operator==(RGB other) {
        return r == other.r && g == other.g && b == other.b;
    };
    bool operator!=(RGB other) {
        return !(r == other.r && g == other.g && b == other.b);
    };
    String toString() {
        String string = String(r);
        string.concat(".");
        string.concat(g);
        string.concat(".");
        string.concat(b);
        return string;
    };
};
#endif
