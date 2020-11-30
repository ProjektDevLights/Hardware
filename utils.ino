
//IP utils (saving,reading,converting)
bool writeIp(IPAddress ip)
{
  int add_w = add_ip;
  for (int i = 0; i < 4; i++)
  {
    EEPROM.write(add_w, ip[i]);
    add_w++;
  }
  if (EEPROM.commit())
  {
    return true;
  }
  else
  {
    return false;
  }
}

IPAddress readIp()
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

String ipToString(IPAddress ip)
{
  return String(ip[0]) + "." + String(ip[1]) +
         "." + String(ip[2]) + "." + String(ip[3]);
}

//color utils
void writeColor(StripColor color)
{
  EEPROM.write(add_col, color.r);
  EEPROM.write(add_col + 1, color.g);
  EEPROM.write(add_col + 2, color.b);
  EEPROM.write(add_pat, color.pattern);
  EEPROM.commit();
}

StripColor readColor()
{
  StripColor color;
  color.r = EEPROM.read(add_col);
  color.g = EEPROM.read(add_col + 1);
  color.b = EEPROM.read(add_col + 2);
  color.pattern = EEPROM.read(add_pat);
  return color;
}

StripColor stringsToColor(String pattern, String colors)
{
  StripColor color;
  StringSplitter *splitter = new StringSplitter(colors, '.', 3);
  color.r = splitter->getItemAtIndex(0).toInt();
  color.g = splitter->getItemAtIndex(1).toInt();
  color.b = splitter->getItemAtIndex(2).toInt();
  if (pattern == "plain")
  {
    color.pattern = 1;
  }
  return color;
}

void setStrip(StripColor color)
{
  pixels.clear();
  if (color.pattern == 1)
  {
    Serial.println(pixelCount);
    pixels.fill(pixels.Color(color.r, color.g, color.b), 0, pixelCount);
  }
  delay(100);
  pixels.show();
}

//ID utils
bool writeId(String id)
{
  int add_w = add_id;
  StringSplitter *splitter = new StringSplitter(id, '.', 2);
  for (int i = 0; i < 2; i++)
  {
    EEPROM.write(add_w, byte(splitter->getItemAtIndex(i).toInt()));
    add_w++;
  }
  if (EEPROM.commit())
  {
    return true;
  }
  else
  {
    return false;
  }
}

String readId()
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

//wifi utils (setup; !!!reconnect!!!)
void setupWifi(String ssid, String password)
{
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.println("...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(++i);
    Serial.print(" ");
  }
  Serial.println("\nConnection established!");
  Serial.println(WiFi.localIP());
}

//JSON utils
DynamicJsonDocument stringToJson(String data)
{
  DynamicJsonDocument doc(2048);
  deserializeJson(doc, data);
  return doc;
}
