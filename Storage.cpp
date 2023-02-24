#include "Storage.h"
// brightness
void Storage::setBrightness(int brightness) {
    EEPROM.write(add_brightness, brightness);
    EEPROM.commit();
}

int Storage::getBrightness() { return EEPROM.read(add_brightness); }

// count
void Storage::setCount(int count) {
    EEPROM.write(add_count, count);
    EEPROM.commit();
}

int Storage::getCount() { return int(EEPROM.read(add_count)); }

// offset
bool Storage::setOffset(int offset) {
    if (offset > Storage::getCount()) return false;
    EEPROM.write(add_offset, offset);
    EEPROM.commit();
    return true;
}

int Storage::getOffset() { return int(EEPROM.read(add_offset)); }

// id
void Storage::setId(String id) {
    int add_w = add_id;
    StringSplitter *splitter = new StringSplitter(id, '.', 2);
    for (int i = 0; i < 2; i++) {
        EEPROM.write(add_w, byte(splitter->getItemAtIndex(i).toInt()));
        add_w++;
    }
    EEPROM.commit();
}

String Storage::getId() {
    byte idBytes[2];
    int add_w = add_id;
    for (int i = 0; i < 2; i++) {
        byte idPart = EEPROM.read(add_w);
        idBytes[i] = idPart;
        add_w++;
    }
    return String(idBytes[0]) + "." + String(idBytes[1]);
}

// ip
void Storage::setIp(IPAddress ip) {
    int add_w = add_ip;
    for (int i = 0; i < 4; i++) {
        EEPROM.write(add_w, ip[i]);
        add_w++;
    }
    EEPROM.commit();
}

IPAddress Storage::getIp() {
    byte ipBytes[4];
    int add_w = add_ip;
    for (int i = 0; i < 4; i++) {
        byte ipPart = EEPROM.read(add_w);
        ipBytes[i] = ipPart;
        add_w++;
    }
    IPAddress ip(ipBytes[0], ipBytes[1], ipBytes[2], ipBytes[3]);
    return ip;
}
// isOn
void Storage::setIsOn(bool isOn) {
    if (isOn) {
        EEPROM.write(add_on, 1);
    } else {
        EEPROM.write(add_on, 0);
    }

    EEPROM.commit();
}

bool Storage::getIsOn() {
    int isOn = EEPROM.read(add_on);
    return isOn == 1;
}

// isSetup
void Storage::setIsSetup(bool isSetup) {
    if (isSetup) {
        EEPROM.write(add_setup, byte(2));
    } else {
        EEPROM.write(add_setup, 0);
    }
    EEPROM.commit();
}
bool Storage::getIsSetup() {
    int isSetupInt = EEPROM.read(add_setup);
    if (isSetupInt == 2) {
        return true;
    } else {
        return false;
    }
}
// stripPattern
void Storage::setStripPattern(StripPattern pattern) {
    int add_w = add_color;

    for (int i = 0; i < 10; i++) {
        EEPROM.write(add_w, pattern.colors[i].r);
        EEPROM.write(add_w + 1, pattern.colors[i].g);
        EEPROM.write(add_w + 2, pattern.colors[i].b);
        add_w += 3;
    }
    char ints[5];
    itoa(pattern.timeout, ints, 10);
    add_w = add_timeout;
    for (int i = 0; i < 5; i++) {
        EEPROM.write(add_w, ints[i]);
        add_w++;
    }
    EEPROM.write(add_pattern, pattern.pattern);
    EEPROM.commit();
}

StripPattern Storage::getStripPattern() {
    StripPattern pattern;
    int add_r = add_color;
    for (int i = 0; i < 10; i++) {
        pattern.colors[i].r = EEPROM.read(add_r);
        pattern.colors[i].g = EEPROM.read(add_r + 1);
        pattern.colors[i].b = EEPROM.read(add_r + 2);
        add_r += 3;
    }
    char ints[5];
    add_r = add_timeout;
    for (int i = 0; i < 5; i++) {
        ints[i] = EEPROM.read(add_r);
        add_r++;
    }

    pattern.pattern = EEPROM.read(add_pattern);
    pattern.timeout = atoi(ints);
    return pattern;
}

void Storage::setCustom(std::vector<RGB> colors) {
    EEPROM.write(add_custom_len, colors.size());

    int work_add = add_custom;
    for (int i = 0; i < colors.size(); i++) {
        Storage::writeColor(colors[i], work_add);
        work_add += 3;
    }
    EEPROM.commit();
}

std::vector<RGB> Storage::getCustom() {
    int len = EEPROM.read(add_custom_len);
    std::vector<RGB> colors;
    colors.resize(len);
    int work_add = add_custom;
    for (int i = 0; i < len; i++) {
        colors[i] = readColor(work_add);
        work_add += 3;
    }
    return colors;
}

void Storage::writeColor(RGB color, int add, bool commit) {
    EEPROM.write(add, color.r);
    EEPROM.write(add + 1, color.g);
    EEPROM.write(add + 2, color.b);

    if (commit) {
        EEPROM.commit();
    }
}

RGB Storage::readColor(int add) {
    RGB color;
    color.r = EEPROM.read(add);
    color.g = EEPROM.read(add + 1);
    color.b = EEPROM.read(add + 2);
    return color;
}

// clear
void Storage::clear() {
    for (int i = 0; i < 2048; i++) {
        EEPROM.write(i, 0x00);
    }
    EEPROM.commit();
}
// print
void Storage::print() {
    for (int i = add_custom_len; i < add_custom_len + 30; i++) {
        Serial.print("Add ");
        Serial.print(i, HEX);
        Serial.print(": ");
        Serial.println(EEPROM.read(i));
    }
}
