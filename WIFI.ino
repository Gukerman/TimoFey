//-------------Подключение к роутеру конфигурация в строке json
//{"ip":"192.168.0.190","getway":"192.168.0.1","subnet":"255.255.255.0","dns":"","checkboxIP":0}
boolean startSTA(String configWiFi) {
  //WiFi.persistent (false);
  // if (jsonRead(configWiFi, "checkboxIP") == "1") {
    // IPAddress staticIP;
    // IPAddress staticGateway;
    // IPAddress staticSubnet;
    // String volume = "";
    // boolean check = true;
    // volume = jsonRead(configWiFi, "ip");
    // if (volume != "") {
      // staticIP.fromString(volume);
      // check = false;
    // }
    // volume = jsonRead(configWiFi, "getway");
    // if (volume != "") {
      // staticGateway.fromString(volume);
      // check = false;
    // }
    // volume = jsonRead(configWiFi, "subnet");
    // if (volume != "") {
      // staticSubnet.fromString(volume);
      // check = false;
    // }
    // if (!check) {
      // WiFi.config (staticIP,staticGateway,staticSubnet);
    // }
  // }
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_STA);
  WiFi.hostname ( SSDP_Name + "-" + chipID );
//  WiFi.begin();
  WiFi.begin(_ssid.c_str(), _password.c_str());   
//WiFi.begin(jsonRead(configJson, "ssid").c_str(),jsonRead(configJson, "ssidPass").c_str());
  if ( wifiConnect(60, pinled)) {
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
//    configSetup = jsonWrite(configSetup, "ip", WiFi.localIP().toString());
//    configSetup = jsonWrite(configSetup, "getway", WiFi.gatewayIP().toString());
//    configSetup = jsonWrite(configSetup, "subnet", WiFi.subnetMask().toString());
//    statistics();
    return true;
  }
  return false;
}
// --------------Проверять на подключение к роутеру  tries раз. Отображать подключение на ноге pin
boolean wifiConnect(byte tries, byte pin) {
  if (tries == 0) tries = 11;
  if ((pin > 5 && pin < 12) || pin > 16) pin = 0 ;
  if (pin != 0)   pinMode(pin, OUTPUT);

//  uint32_t millis_wifi = millis() + 500;
   
  while (--tries && WiFi.status() != WL_CONNECTED)
  {
    //Мигаем сетодиодом при попытке подключится к роутеру
    
    if (pin != 0)   digitalWrite(pin, HIGH);
    delay(500);
    if (pin != 0)  digitalWrite(pin, LOW);
    delay(500);

    Serial.print(".");
    lcd.home();                // At column=0, row=0
    lcd.print("WiFi connect");   
    lcd.setCursor(0, 1);
    lcd.print(tries);   
    lcd.print("        ");   
 
 /*      
    if ((int32_t)millis() >= (int32_t)millis_wifi)
    {
    Serial.println(tries);
      --tries;
      millis_wifi = millis() + 500;
    }
    
//    if (500 < millis()- millis_wifi)  --tries;
/*    {
      tries=tries-1;
      millis_wifi=millis();

    if (pin != 0)   digitalWrite(pin, !digitalRead(pin));

    Serial.println(tries);

    lcd.home();                // At column=0, row=0
    lcd.print("WiFi connect");   
    lcd.setCursor(0, 1);
    lcd.print(tries);   
    lcd.print("        ");   
    }
    filtr();      
/*  
 *   
 */
 }
  
  if (WiFi.status() == WL_CONNECTED)return true;
  return false;
}

//-------------Включение режима AP конфигурация в строке json
//{"ssidAP":"Sonoff","ssidApPass":""}
boolean startAP(String configWiFi) {
  IPAddress apIP(192, 168, 4, 1);
  IPAddress staticGateway(192, 168, 4, 1);
  IPAddress staticSubnet(255, 255, 255, 0);
  WiFi.softAPConfig(apIP, staticGateway, staticSubnet);
  WiFi.softAP(_ssidAP.c_str(), _passwordAP.c_str());
//  WiFi.softAP(jsonRead(configWiFi, "ssidAP").c_str(), jsonRead(configWiFi, "ssidApPass").c_str());
  WiFi.mode(WIFI_AP);
//  dnsServer.start(53, "*", apIP);
  //Зажигаем светодиод если находимся в режиме AP
  int pin = pinled;
  if ((pin > 5 && pin < 12) || pin > 16) pin = 0 ;
  if (pin != 0)  {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
  }
  return true;
}


// ----------------- Запускаем WiFi
void WIFIinit() {
  if (startSTA(configSetup)) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    startAP(configSetup);
    Serial.println("");
    Serial.println("WiFi up AP");  
  }
}


