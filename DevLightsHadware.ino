#include <ESP8266WiFi.h>
#include <SPI.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include "Adafruit_NeoPixel.h"
#include <EEPROM.h>
#include <HttpClient.h>
#include <WiFiClient.h>
#include "StringSplitter.h"

//global variables
WiFiServer server(2389);

//setup neopixels
#ifdef __AVR__
#include <avr/power.h>
#endif
#define PIN 5 //D1
#define PIXELFORMAT NEO_GRB + NEO_KHZ800
uint16_t pixelCount = 300;
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
clock_prescale_set(clock_div_1);
#endif

#define DELAYVAL 1000
Adafruit_NeoPixel pixels(pixelCount, PIN, PIXELFORMAT);

//define colors
uint32_t standartColor = pixels.Color(29, 233, 182);

//network consts
const int nTimeout = 30 * 1000;
const int nDelay = 1000;

//eeprom addresses
const int add_id = 0x40;
const int add_setup = 0x02;
const int add_ip = 0x80;
const int add_count = 0x10;
const int add_pat = 0x20;
const int add_col = 0x25;

/**PATTERN CONSTANTS 
 * 1 = plain,
 * 
 */
struct StripColor
{
  int r;
  int g;
  int b;
  int pattern;
};

void setup()
{
  //basic debugging setup -------------------------------------------------------
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.begin(115200);

  Serial.println("\n");
  Serial.println("\n");
  Serial.println("\n");
  Serial.println("------------------------");

  //determine if esp needs to enter setup mode ----------------------------------
  EEPROM.begin(512);
  byte isSetup = EEPROM.read(add_setup);
  //isSetup == 0x01   //true
  if (isSetup == 0x01)
  {
    Serial.println("Run Setup");
    runSetup();
  }
  else
  {
    Serial.println("First Setup");
    firstSetup();
  }

  //start tcp server to receive data
  server.begin();
  server.setNoDelay(true);

  //-----------------------------------------------------------------------------
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop()
{
  DynamicJsonDocument data = stringToJson(readTcpData());
  if (data["command"] == "leds")
  {
    StripColor color = stringsToColor(data["data"]["pattern"], data["data"]["colors"][0]);
    setStrip(color);
    writeColor(color);
    Serial.println("show pixels");
  }
  if (data["command"] == "reset")
  {
    for (int i = 0; i < 512; i++)
    {
      EEPROM.write(i, 0x00);
    }
    pixels.clear();
    pixels.show();
    EEPROM.commit();
    delay(200);
    ESP.restart();
  }
  if (data["command"] == "restart")
  {
    pixels.clear();
    pixels.show();
    delay(200);
    Serial.println("restarting...");
    ESP.restart();
  }
  if (data["command"] == "count")
  {
    String countString = data["data"];
    uint16_t count = countString.toInt();
    Serial.println(count);
    Serial.println(byte(count));
    if (count >= 0)
    {
      Serial.println("if");
      EEPROM.write(add_count, byte(count));
      EEPROM.commit();
      pixels.clear();
      pixels.show();
      pixels.updateLength(count);
      pixelCount = count;
      delay(50);
    }
  }
  if (data["command"] == "brightness")
  {
    int brightness = int(data["data"]);
    Serial.println(brightness);
    pixels.setBrightness(brightness);
    delay(100);
    Serial.println("set brightness");
  }
  if (data["command"] == "off")
  {
    pixels.clear();
    pixels.show();
  }
  if (data["command"] == "on")
  {
    setStrip(readColor());
    pixels.show();
  }
}
