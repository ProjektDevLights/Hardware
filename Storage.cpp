#include "Storage.h"

bool Storage::getIsSetup()
{
    int isSetupInt = EEPROM.read(add_setup);
    if (isSetupInt == 2)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Storage::setIsSetup(bool isSetup)
{
    if (isSetup)
    {
        EEPROM.write(add_setup, byte(2));
    }
    else
    {
        EEPROM.write(add_setup, 0);
    }
    delay(50);
    EEPROM.commit();
    delay(50);
}

String Storage::getId()
{
    byte idBytes[2];
    int add_w = add_id;
    for (int i = 0; i < 2; i++)
    {
        byte idPart = EEPROM.read(add_w);
        idBytes[i] = idPart;
        add_w++;
    }
    return String(idBytes[0]) + "." + String(idBytes[1]);
}

void Storage::setId(String id)
{
    int add_w = add_id;
    StringSplitter *splitter = new StringSplitter(id, '.', 2);
    for (int i = 0; i < 2; i++)
    {
        EEPROM.write(add_w, byte(splitter->getItemAtIndex(i).toInt()));
        add_w++;
    }
    EEPROM.commit();
}

IPAddress Storage::getIp()
{
    byte ipBytes[4];
    int add_w = add_ip;
    for (int i = 0; i < 4; i++)
    {
        byte ipPart = EEPROM.read(add_w);
        ipBytes[i] = ipPart;
        add_w++;
    }
    IPAddress ip(ipBytes[0], ipBytes[1], ipBytes[2], ipBytes[3]);
    return ip;
}

void Storage::setIp(IPAddress ip)
{
    int add_w = add_ip;
    for (int i = 0; i < 4; i++)
    {
        EEPROM.write(add_w, ip[i]);
        add_w++;
    }
    EEPROM.commit();
}

int Storage::getCount()
{
    return int(EEPROM.read(add_count));
}

void Storage::setCount(int count)
{
    EEPROM.write(add_count, count);
    EEPROM.commit();
}

StripPattern Storage::getStripPattern()
{
    StripPattern pattern;
    int add_r = add_color;
    for (int i = 0; i < 10; i++)
    {
        pattern.colors[i].r = EEPROM.read(add_r);
        pattern.colors[i].g = EEPROM.read(add_r + 1);
        pattern.colors[i].b = EEPROM.read(add_r + 2);
        add_r += 3;
    }
    pattern.pattern = EEPROM.read(add_pattern);
    return pattern;
}

void Storage::setStripPattern(StripPattern pattern)
{
    int add_w = add_color;

    for (int i = 0; i < 10; i++)
    {
        EEPROM.write(add_w, pattern.colors[i].r);
        EEPROM.write(add_w + 1, pattern.colors[i].g);
        EEPROM.write(add_w + 2, pattern.colors[i].b);
        add_w += 3;
    }
    EEPROM.write(add_pattern, pattern.pattern);
    EEPROM.commit();
}

void Storage::clear()
{
    for (int i = 0; i < 512; i++)
    {
        EEPROM.write(i, 0x00);
    }
    EEPROM.commit();
}

void Storage::print()
{
    for (int i = 0; i < 0x2b; i++)
    {
        Serial.print("Add ");
        Serial.print(i);
        Serial.print(": ");
        Serial.println(EEPROM.read(i));
    }
}
