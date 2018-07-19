
void HTTP_init(void) {

  HTTP.on("/configs.json", handle_ConfigJSON); // формирование configs.json страницы для передачи данных в web интерфейс
  // API для устройства
  HTTP.on("/ssdp", handle_Set_Ssdp);     // Установить имя SSDP устройства по запросу вида /ssdp?ssdp=proba
  HTTP.on("/ssid", handle_Set_Ssid);     // Установить имя и пароль роутера по запросу вида /ssid?ssid=home2&password=12345678
  HTTP.on("/ssidap", handle_Set_Ssidap); // Установить имя и пароль для точки доступа по запросу вида /ssidap?ssidAP=home1&passwordAP=8765439
  HTTP.on("/filtr", handle_Set_filtr); // Установить код геркона по запросу вида  http://192.168.0.101/filtr?filtr=2866731
  HTTP.on("/restart", handle_Restart);   // Перезагрузка модуля по запросу вида /restart?device=ok
  HTTP.on("/mqtt", handle_Set_MQTT);

  // Добавляем функцию Update для перезаписи прошивки по WiFi при 1М(256K SPIFFS) и выше
  httpUpdater.setup(&HTTP);
  // Запускаем HTTP сервер
  HTTP.begin();
}

// Функции API-Set
// Установка SSDP имени по запросу вида http://192.168.0.101/ssdp?ssdp=proba
void handle_Set_Ssdp() {
  SSDP_Name = HTTP.arg("ssdp"); // Получаем значение ssdp из запроса сохраняем в глобальной переменной
  saveConfigSetup();                 // Функция сохранения данных во Flash
  HTTP.send(200, "text/plain", "OK"); // отправляем ответ о выполнении
}
// Установка параметров для подключения к внешней AP по запросу вида http://192.168.0.101/ssid?ssid=home2&password=12345678
void handle_Set_Ssid() {
  _ssid = HTTP.arg("ssid");            // Получаем значение ssid из запроса сохраняем в глобальной переменной
  _password = HTTP.arg("password");    // Получаем значение password из запроса сохраняем в глобальной переменной
  saveConfigSetup();                        // Функция сохранения данных во Flash
  HTTP.send(200, "text/plain", "OK");   // отправляем ответ о выполнении
}
//Установка параметров внутренней точки доступа по запросу вида http://192.168.0.101/ssidap?ssidAP=home1&passwordAP=8765439
void handle_Set_Ssidap() {              //
  _ssidAP = HTTP.arg("ssidAP");         // Получаем значение ssidAP из запроса сохраняем в глобальной переменной
  _passwordAP = HTTP.arg("passwordAP"); // Получаем значение passwordAP из запроса сохраняем в глобальной переменной
  saveConfigSetup();                         // Функция сохранения данных во Flash
  HTTP.send(200, "text/plain", "OK");   // отправляем ответ о выполнении
}
// Перезагрузка модуля по запросу вида http://192.168.0.101/restart?device=ok
void handle_Restart() {
  String restart = HTTP.arg("device");          // Получаем значение device из запроса
  if (restart == "ok") {                         // Если значение равно Ок
    HTTP.send(200, "text / plain", "Reset OK"); // Oтправляем ответ Reset OK
    ESP.restart();                                // перезагружаем модуль
  }
  else {                                        // иначе
    HTTP.send(200, "text / plain", "No Reset"); // Oтправляем ответ No Reset
  }
}

// Установить код геркона по запросу вида  http://192.168.0.101/filtr?filtr=2866731
void handle_Set_filtr() 
{              //

  filtrV = HTTP.arg("filtrV");         // Получаем значение ssidAP из запроса сохраняем в глобальной переменной
  filtrT = HTTP.arg("filtrT");         // Получаем значение ssidAP из запроса сохраняем в глобальной переменной
  vodomerIn = HTTP.arg("vodomerIn");         // Получаем значение ssidAP из запроса сохраняем в глобальной переменной
  vodomerOut = HTTP.arg("vodomerOut");         // Получаем значение ssidAP из запроса сохраняем в глобальной переменной
  filtrVreal = HTTP.arg("filtrVreal");         // Получаем значение ssidAP из запроса сохраняем в глобальной переменной
  filtrTdata = HTTP.arg("filtrTdata");         // Получаем значение ssidAP из запроса сохраняем в глобальной переменной
  saveConfigSetup();                         // Функция сохранения данных во Flash
  HTTP.send(200, "text/plain", "OK");   // отправляем ответ о выполнении

  Serial.print("filtrV - "); Serial.println(filtrV); 
  Serial.print("filtrT - "); Serial.println(filtrT); 
  Serial.print("vodomerIn - "); Serial.println(vodomerIn); 
  Serial.print("vodomerOut - "); Serial.println(vodomerOut);   
  Serial.print("filtrVreal - "); Serial.println(filtrVreal); 
  Serial.print("filtrTdata - "); Serial.println(filtrTdata); 
  Summ1 = filtrVreal.toInt();
  
  Year_i = filtrTdata.substring(6, 10).toInt(); // Выделяем год
  Month_i = filtrTdata.substring(3, 5).toInt(); // Выделяем месяц
  Day_i = filtrTdata.substring(0, 2).toInt(); // Выделяем день

  filtr();
  Led();      //Функция отрисовки экрана

  handle_Time();
  handleMQTT();   
  
}

void handle_ConfigJSON() {
  String root = "{}";  // Формировать строку для отправки в браузер json формат
  //{"SSDP":"SSDP-test","ssid":"home","password":"i12345678","ssidAP":"WiFi","passwordAP":"","ip":"192.168.0.101"}
  // Резервируем память для json обекта буфер может рости по мере необходимти, предпочтительно для ESP8266
  DynamicJsonBuffer jsonBuffer;
  //  вызовите парсер JSON через экземпляр jsonBuffer
  JsonObject& json = jsonBuffer.parseObject(root);
  // Заполняем поля json
  json["SSDP"] = SSDP_Name;
  json["ssidAP"] = _ssidAP;
  json["passwordAP"] = _passwordAP;
  json["ssid"] = _ssid;
  json["password"] = _password;
  json["timezone"] = timezone;
  json["ip"] = WiFi.localIP().toString();
  json["time"] = GetTime();
  json["date"] = GetDate();
  json["filtrV"] = filtrV;
  json["filtrT"] = filtrT;
  json["vodomerIn"] = vodomerIn;
  json["vodomerOut"] = vodomerOut;
  json["filtrVreal"] = filtrVreal;
  Summ1 = filtrVreal.toInt();  
  json["filtrTdata"] = filtrTdata;
  json["mqttServer"] = mqttServer;     
  json["mqttPort"] = mqttPort;     
  json["mqttUser"] = mqttUser;     
  json["mqttPass"] = mqttPass;     

  // Помещаем созданный json в переменную root
  root = "";
  json.printTo(root);
  HTTP.send(200, "text/json", root);
}




