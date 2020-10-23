void runSetup()
{

  //Setup LedStrip--------------------------------------------------------------
  pixels.begin();
  pixels.setBrightness(127);

  //Setup Wifi-------------------------------------------------------------------
  //TODO read from eeprom
  String ssid = "devlight";
  String password = "HatJaNur5StundenGedauert";
  setupWifi(ssid, password);

  //falls sich ip ändert beim server anmelden
  WiFiClient c;
  HttpClient client(c, "TimoLaptop.local", 80);
  int err = 0;
  //TODO save and read id
  String id = readId();
  String request = "/esp/update?ip=" + ipToString(WiFi.localIP()) + "&id=" + id;
  err = client.get(request);
  if (err == 0)
  {
    err = client.responseStatusCode();
    if (err == 200)
    {
      writeIp(WiFi.localIP());
    } else {
      Serial.println("error updating ip 1");
    }
  } else {
    Serial.println("error updating ip 2");
  }
  //pixel count und farbe lesen und setzen
  int count = int(EEPROM.read(add_count));
  Serial.println(count);
  pixels.updateLength(count);
  pixels.clear();
  pixels.show();
  pixelCount = count;
}
