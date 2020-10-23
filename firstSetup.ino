void firstSetup()
{

  //Setup Wifi connection ------------------------------------------------------
  String ssid = "devlight";
  String password = "HatJaNur5StundenGedauert";
  setupWifi(ssid, password);

  //Setup random generator -------------------------------------------------------
  randomSeed(analogRead(0));
  delay(10);

  //Beim Server anmelden
  WiFiClient c;
  HttpClient client(c, "TimoLaptop.local", 80);

  int err = 0;
  String request = "/esp/setup?ip=";
  request += ipToString(WiFi.localIP());
  err = client.get(request);
  if (err == 0)
  {
    err = client.responseStatusCode();
    if (err == 200)
    {
      err = client.skipResponseHeaders();
      if (err >= 0)
      {
        int bodyLength = client.contentLength();
        unsigned long timeoutStart = millis();
        String response = "";
        char c;
        while ((client.available() || client.connected()) && ((millis() - timeoutStart) < nTimeout))
        {
          if (client.available())
          {
            c = client.read();
            response += c;
            bodyLength--;
            timeoutStart = millis();
          }
          else
          {
            delay(nDelay);
          }
        }
        writeId(response);
        Serial.println("succesfully registered");
      }
    } else {
      Serial.println("someone stole my ip");
    }
  } else {
    Serial.println("someone stole my ip");
  }
  //just do when registered should occur otherwise
  EEPROM.write(add_setup, 0x01);
  writeIp(WiFi.localIP());

  pixels.begin();
  pixels.fill(standartColor, 0 , pixelCount);
  pixels.show();
  pixels.updateLength(0);
  pixelCount = 0;
}
