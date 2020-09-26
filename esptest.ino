#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Ethernet.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
#define PIN        5 //D1
#define NUMPIXELS 150
#define USE 6

#define DELAYVAL 1000
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 1000
uint32_t off = pixels.Color(0, 0, 0); //0
uint32_t white = pixels.Color(150, 150, 150); //1
uint32_t red = pixels.Color(150, 0, 0);//2
uint32_t green = pixels.Color(0, 150, 0);//3
uint32_t blue = pixels.Color(0, 0, 150);//4
uint32_t cyan = pixels.Color(0, 150, 150);//5
uint32_t yellow = pixels.Color(150, 150, 0);//6
uint32_t magenta = pixels.Color(150, 0, 150);//7

uint32_t colors[] = {off, white, red, green, blue, cyan, yellow, magenta};




WiFiServer server(2389);

const char* ssid = "devlights";
const char* password = "ChangeMe";

String message = "";


void setup() {

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  pixels.begin();
  pixels.setBrightness(64);
  randomSeed(analogRead(0));

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.begin(115200);
  delay(10);
  Serial.print("\n\n");

  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.println("...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(++i);
    Serial.print(" ");
  }
  Serial.println("\nConnection established!");
  Serial.println( WiFi.localIP());

  server.begin();
  server.setNoDelay(true);
  String randomHostname = randomString();

  if (!MDNS.begin("esp" + randomHostname)) {
    Serial.println("Error setting up MDNS responder!");
  }

  Serial.println("mDNS responder started: esp" + randomHostname);

  digitalWrite(LED_BUILTIN, HIGH);

}

String randomString() {
  String string = "";
  for (int i = 0; i < 6; i++) {
    string += String(random(0, 9));
  }
  return string;




}

DynamicJsonDocument readData() {

  DynamicJsonDocument doc(2048);

  WiFiClient client = server.available();
  if (!client) {
    return doc;
  }
  if (!client.connected()) {
    return doc;
  }
  String readString = "";
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      if (c == '\n') {
        break;
      }
      readString += c;
    }
  }
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);
  while (!readString.startsWith("{")) {
    readString = readString.substring(3);
  }
  Serial.println(readString);
  client.println("Ending connection");
  client.stop();

  deserializeJson(doc, readString);
  return doc;
}


void loop()
{
  MDNS.update();
  DynamicJsonDocument data(2048);
  data = readData();
  if (data["pattern"]) {
    int color = int(data["data"]);
    Serial.println(color);
    pixels.clear();
    pixels.setPixelColor(1, colors[color]);
    delay(100);
    pixels.show();
    Serial.println("show pixels");
  }
}
