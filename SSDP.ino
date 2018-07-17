void initSSDP(void) {
  // SSDP дескриптор
    if (WiFi.status() == WL_CONNECTED) {     
  
  HTTP.on("/description.xml", HTTP_GET, []() {
  SSDP.schema(HTTP.client());
  });
  //Если версия  2.0.0 закаментируйте следующую строчку
  SSDP.setDeviceType("upnp:rootdevice");
  SSDP.setSchemaURL("description.xml");
  SSDP.setHTTPPort(80);
  SSDP.setName(SSDP_Name);
  SSDP.setSerialNumber(chipID);
  SSDP.setURL("/");
  SSDP.setModelName("filtr-Test");
  SSDP.setModelNumber(chipID);
  SSDP.setModelURL("http://Ufanet.ru");
  SSDP.setManufacturer("Guk");
  SSDP.setManufacturerURL("http://www.Ufanet.ru");
  SSDP.begin();
  }
}
