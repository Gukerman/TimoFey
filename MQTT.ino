/*
    Для использования вкладки требуется добавить в заголовке скетча следуюший код
  #include <PubSubClient.h>
  WiFiClient wclient;
  PubSubClient client(wclient);
*/

void callback(const MQTT::Publish& sub) { //Есть запись в топике
  Serial.print("Get data from subscribed topic ");
  Serial.print(sub.topic());  // Название топика
  Serial.print(" => ");
  Serial.println(sub.payload_string()); // Данные из топика

  if  (sub.topic() == "/Update")  {

      Serial.print("***************Update");

 //   bin = sub.payload_string();
    if (bin != "") webUpdate();
      
  }

  // echo
  MQTT::Publish newpub(pubTopic, sub.payload(), sub.payload_len());
  client.publish(newpub);
}

void initMQTT() {
//  modulesReg("mqtt");
  MQTT_Pablush();
}

void MQTT_Pablush() {

  if ((mqttServer != "")) {
    if (WiFi.status() == WL_CONNECTED) {     
      
      client.set_callback(callback);

      if (client.connect(MQTT::Connect(chipID).set_auth(mqttUser, mqttPass))) 
          {
            Serial.println("Connected to MQTT broker");
            client.subscribe(controlTopic);
            Serial.println("subscribe "+controlTopic);
          } else {
            Serial.println("Will reset and try again...");
           // abort();
          }  
     }
  }

/*
  if ((mqttServer != "")) {
    client.set_server(mqttServer, mqttPort.toInt());
    // подключаемся к MQTT серверу
    if (WiFi.status() == WL_CONNECTED) {     
      if (!client.connected()) {
        if (client.connect(MQTT::Connect(chipID)
                           .set_auth(mqttUser, mqttPass))) {
                             Serial.print("MQTT_Pablush - ");
          Serial.println(mqttServer); 
          
          client.set_callback(callback);
          //client.subscribe(prefix);  // Для приема получения HELLOW и подтверждения связи
          client.subscribe("/+/+/Update"); // Подписываемся на топики Update
          //client.subscribe("/" + chipID + "/Update"); // подписываемся по топик с данными для светодиода
 //         loadnWidgets();
 
 Serial.print("MQTT - ");
 Serial.println(mqttServer); 
        } else {
          
        }
      }
    }
  }
*/
}

void  handleMQTT() {

   // if  ( filtrVreal.toInt() != Summ1)    
    {
    publishMQTT(pubTopic+mqttUser+"/vodomerOut", vodomerOut);
    publishMQTT(pubTopic+mqttUser+"/filtrTdata", filtrTdata);
    publishMQTT(pubTopic+mqttUser+"/filtrV", filtrV);
    publishMQTT(pubTopic+mqttUser+"/filtrT", filtrT);
    publishMQTT(pubTopic+mqttUser+"/vodomerIn", vodomerIn);
    publishMQTT(pubTopic+mqttUser+"/FreeHeap", String(FreeHeap));
    }
    publishMQTT(pubTopic+mqttUser+"/filtrVreal", filtrVreal);
//    publishMQTT(pubTopic+mqttUser+"/Status", Status);
     
}

void  publishMQTT(String pubTopic, String cTopic) 
{

  if (client.connected()) client.loop();
  else {
    MQTT_Pablush();
    //Serial.println("MQTT");
    }
    
  if ( mult != cTopic)
  {
          //Публикуем ReceivedValue в топике
          if (client.publish(pubTopic,  cTopic)) {
            Serial.print(" Publish " + pubTopic + " - ");
            Serial.println(cTopic);
          }
          else {
            Serial.println(" Publish failed");
          }
          mult = cTopic;
  }
}          

//Установка параметров  http://192.168.0.101/mqtt?server=m13.cloudmqtt.com&port=15535&user=cxluynva&pass=4cje5WEkzqvR
void handle_Set_MQTT() {              //

  mqttServer = HTTP.arg("mqttServer");         // Получаем значение mqttServer из запроса сохраняем в глобальной переменной
  mqttPort = HTTP.arg("mqttPort");         // Получаем значение mqttPort из запроса сохраняем в глобальной переменной
  mqttUser = HTTP.arg("mqttUser");         // Получаем значение mqttUser из запроса сохраняем в глобальной переменной
  mqttPass = HTTP.arg("mqttPass");         // Получаем значение mqttPass из запроса сохраняем в глобальной переменной
  saveConfigSetup();                         // Функция сохранения данных во Flash
  MQTT_Pablush();
  HTTP.send(200, "text/plain", "OK");   // отправляем ответ о выполнении

  Serial.print("mqttServer - "); Serial.println(mqttServer); 
  Serial.print("mqttPort - "); Serial.println(mqttPort); 
  Serial.print("mqttUser - "); Serial.println(mqttUser); 
  Serial.print("mqttPass - "); Serial.println(mqttPass);   
}



