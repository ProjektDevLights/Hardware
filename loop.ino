String readTcpData()
{
  //read data from server if available ---------------------------------
  WiFiClient client = server.available();
  if (!client)
  {
    return "";
  }
  if (!client.connected())
  {
    return "";
  }
  String readString = "";
  while (client.connected())
  {
    if (client.available())
    {
      char c = client.read();
      if (c == '\n')
      {
        break;
      }
      readString += c;
    }
  }

  //indicate incomming data (debug);
  digitalWrite(LED_BUILTIN, LOW);
  delay(50);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println(readString);
  client.println("Ending connection");
  Serial.println("Ending connection");
  client.stop();

  return readString;
}
