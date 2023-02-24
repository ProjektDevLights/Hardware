#include "Arduino.h"
#include "EEPROM.h"
#include "ESP8266WiFi.h"
#include "StringSplitter.h"
#include "StripPattern.h"

#ifndef Storage_h
#define Storage_h
class Storage {
   public:
    static void setBrightness(int brightness);
    static int getBrightness();
    static void setCount(int count);
    static int getCount();
    static bool setOffset(int offset);
    static int getOffset();
    static void setId(String id);
    static String getId();
    static void setIp(IPAddress ip);
    static IPAddress getIp();
    static void setIsOn(bool isOn);
    static bool getIsOn();
    static void setIsSetup(bool isSetup);
    static bool getIsSetup();
    static void setStripPattern(StripPattern color);
    static StripPattern getStripPattern();
    static void setCustom(std::vector<RGB> colors);
    static std::vector<RGB> getCustom();
    static void writeColor(RGB color, int add, bool commit = false);
    static RGB readColor(int add);
    static void clear();
    static void print();

   private:
    static const int add_brightness = 0x01;  // length: 1
    static const int add_color = 0x02;       // length: 3*10 = 30
    static const int add_count = 0x21;       // length: 1
    static const int add_id = 0x23;          // length: 2
    static const int add_ip = 0x25;          // length: 4
    static const int add_on = 0x29;          // length: 1
    static const int add_pattern = 0x2a;     // length: 1
    static const int add_setup = 0x2b;       // length: 1
    static const int add_timeout = 0x2c;     // length: 5
    static const int add_custom_len = 0x31;  // length: 1
    static const int add_custom = 0x32;      // length: 1500 next: 0x060F
    static const int add_offset = 0x22;    // length: 1
};
#endif
