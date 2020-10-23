
//IP utils (saving,reading,converting)
bool writeIp(IPAddress ip) {
  int add_w = add_ip;
  for (int i = 0 ; i < 4; i++) {
    EEPROM.write(add_w, ip[i]);
    add_w++;
  }
  if(EEPROM.commit()){
    return true;
  } else {
    return false;
  }
}

IPAddress readIp() {
  byte ipBytes[4];
  int add_w = add_ip;
  for (int i = 0 ; i < 4; i++) {
    byte ipPart = EEPROM.read(add_w);
    ipBytes[i] = ipPart;
    add_w++;
  }
  IPAddress ip(ipBytes[0], ipBytes[1], ipBytes[2], ipBytes[3]);
  return ip;
}

String ipToString(IPAddress ip) {
  return String(ip[0]) + "." + String(ip[1]) + \
         "." + String(ip[2]) + "." + String(ip[3]);
}


//ID utils

bool writeId(String id){
  int add_w = add_id;
  StringSplitter *splitter = new StringSplitter(id, '.', 2);
  for (int i = 0 ; i < 2; i++) {
    EEPROM.write(add_w, byte(splitter->getItemAtIndex(i).toInt()));
    add_w++;
  }
  if(EEPROM.commit()){
    return true;
  } else {
    return false;
  }
}


String readId() {
  byte idBytes[2];
  int add_w = add_id;
  for (int i = 0 ; i < 2; i++) {
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
DynamicJsonDocument stringToJson(String data){
  DynamicJsonDocument doc(2048);
  deserializeJson(doc, data);
  return doc;
}
