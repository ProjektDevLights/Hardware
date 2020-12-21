#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "StringSplitter.h"
#include "EEPROM.h"
#include "StripPattern.h"

#ifndef Storage_h
#define Storage_h
class Storage
{
public:
    static bool getIsSetup();
    static String getId();
    static IPAddress getIp();
    static int getCount();
    static StripPattern getStripPattern();
    static void setIsSetup(bool isSetup);
    static void setId(String id);
    static void setIp(IPAddress ip);
    static void setCount(int count);
    static void setStripPattern(StripPattern color);
    static void clear();
    static void print();

private:
    static const int add_setup = 0x0100; //length: 1
    static const int add_id = 0x02;      //length: 2
    static const int add_ip = 0x04;      //length: 4
    static const int add_count = 0x08;   //length: 1
    static const int add_color = 0x09;   // length: 3*10 = 30
    static const int add_pattern = 0x28; //length: 1
};
#endif