/*


*/
#include <WiFiClient.h>
#include <ESP8266mDNS.h> 

#include <ESP8266WiFi.h>        //Содержится в пакете. Видео с уроком http://esp8266-arduinoide.ru/step1-wifi
#include <ESP8266WebServer.h>   //Содержится в пакете. Видео с уроком http://esp8266-arduinoide.ru/step2-webserver
#include <ESP8266SSDP.h>        //Содержится в пакете. Видео с уроком http://esp8266-arduinoide.ru/step3-ssdp
#include <FS.h>                 //Содержится в пакете. Видео с уроком http://esp8266-arduinoide.ru/step4-fswebserver
//                    ПЕРЕДАЧА ДАННЫХ НА WEB СТРАНИЦУ. Видео с уроком http://esp8266-arduinoide.ru/step5-datapages/
//                    ПЕРЕДАЧА ДАННЫХ С WEB СТРАНИЦЫ.  Видео с уроком http://esp8266-arduinoide.ru/step6-datasend/
#include <ArduinoJson.h>        //Установить из менеджера библиотек.
//                    ЗАПИСЬ И ЧТЕНИЕ ПАРАМЕТРОВ КОНФИГУРАЦИИ В ФАЙЛ. Видео с уроком http://esp8266-arduinoide.ru/step7-fileconfig/
#include <ESP8266HTTPUpdateServer.h>  //Содержится в пакете.

#include <Wire.h>  // This library is already built in to the Arduino IDE
#include <LiquidCrystal_I2C.h> //This library you can add via Include Library > Manage Library > 
  
  LiquidCrystal_I2C lcd(0x27,16,2); // Check I2C address of LCD, normally 0x27 or 0x3F
  
 uint8_t heart[8] = {0x0,0xa,0x1f,0x1f,0xe,0x4,0x0};  // example sprite bitmap



// MQTT
#include <PubSubClient.h>
#include <MQTT.h>

WiFiClient espClient;
IPAddress MQTTserver(77, 79, 191, 226); // mosquitto address
PubSubClient client(espClient, MQTTserver);
char* mqtt_user = "User";
char* mqtt_pass = "Pass";
String mqtt_client = "ESP8266";
String mult = "";        // переменная хронения предыдущего сообщания (убирает дубли публикации, но это не точно)
  
// Объект для обнавления с web страницы 
ESP8266HTTPUpdateServer httpUpdater;

// Web интерфейс для устройства
ESP8266WebServer HTTP;

// Для файловой системы
File fsUploadFile;

// Определяем переменные wifi
String _ssid     = ""; // Для хранения SSID
String _password = ""; // Для хранения пароля сети
String _ssidAP = "";   // SSID AP точки доступа
String _passwordAP = ""; // пароль точки доступа
String SSDP_Name = ""; // Имя SSDP
int timezone = 5;               // часовой пояс GTM

String filtrV = "";       // код геркона отрыт 2866734
String filtrT = "";       // код геркона закрыт 2866727
String filtrTdata = "";       // код геркона закрыт 2866727
String filtrVreal = "";       // код геркона закрыт 2866727
String vodomerIn = "";       // код геркона батарейка  2866731
String vodomerOut = "";       // код геркона авария 2866731

String Status = "Ok";
  
String  mqttServer = "";   // 77.79.191.226
String  mqttPort =  "";     // 
String  mqttUser =  "";     // 
String  mqttPass =  "";     // 

String pubTopic = "SMARTHOUSE/";       //smart_house/<номер_договора>/<код_устройства>/<статус_устройства>​ . Например: smart_house/72357995ASU/Door1/OPEN
String cTopic = "";       // состояние геркона для публикации, например OPEN
String controlTopic = "/ESP8266/CONTROL/#";

boolean secTest = true;
//StringCommand sCmd;     // The demo StringCommand object
String command = "";

String chipID = "";
String jsonConfig = "{}";           // Здесь все статусы 
//String configJson = "{}";            // разобраться одну упразнить 

String configOptions = "{}";         // Здесь опции для всех страниц
String configSetup = "{}";           // Здесь данные для setup


int FreeHeap=0; 
int port = 80;


bool flag1 = false;        // Флаг что импульс посчитан
long Summ1 = 0;            // Количество импульсов
int pinIn = 0;           // Порт импульсного входа gpio0
int ad = 200;           // Время для проверки антидребезга в мс
long millis_ad1 = 0;     // переменная для хранения millis() проверки антидребезга

String pr="";

int pinOut = 1; 

int filtrP = 0; 


int pinrele = 5; 
int pinled = 2; 

 
void setup() {


    Serial.begin(115200);
    lcd.begin(4,5);      // In ESP8266-01, SDA=0, SCL=2               
    lcd.backlight();
    lcd.createChar(1, heart);

    
  pinMode (pinrele, OUTPUT);
  pinMode (pinled, OUTPUT);

  digitalWrite(pinrele,LOW);
  digitalWrite(pinled,LOW);


//  HTTP = ESP8266WebServer (port);
  ESP8266WebServer (port);
  Serial.begin(115200);
  Serial.println("");


  chipID = String( ESP.getChipId() ) + "-" + String( ESP.getFlashChipId() );

  //Запускаем файловую систему
  Serial.println("Start 1 FS");
  FS_init();         // Включаем работу с файловой системой

  // ----------------- начинаем загрузку
//  configSetup = readFile("config.save.json", 4096);
/*  configSetup = jsonWrite(configSetup, "time", "00:00:00");
  configJson = jsonWrite(configJson, "setIndex", jsonRead(configSetup, "setIndex"));
  configOptions = jsonWrite(configOptions, "setIndex", jsonRead(configSetup, "setIndex"));
  configOptions = jsonWrite(configOptions, "SSDP", jsonRead(configSetup, "SSDP"));
  String configs = jsonRead(configSetup, "configs");
  configs.toLowerCase();
 
   // ----------- запускаем необходимые всегда модули
     // ----------- Выполняем запуск кофигурации
  configSetup = jsonWrite(configSetup, "mac", WiFi.macAddress().c_str());
  configSetup = jsonWrite(configSetup, "ip", WiFi.localIP().toString());
*/
  
  Serial.println("Step 2 FileConfig");
  loadConfig();
  Serial.println("Step 3 WIFI");
  //Запускаем WIFI
  WIFIinit();
  Serial.println("Step 4 Time");
  // Получаем время из сети
  Time_init();
  //Настраиваем и запускаем SSDP интерфейс
  Serial.println("Step 5 SSDP");
  initSSDP();
  //Настраиваем и запускаем HTTP интерфейс
  Serial.println("Step 6 WebServer");
  HTTP_init();
  
//  mySwitch.enableReceive(pin433);  // Receiver on pin #13
  Serial.println("Step 7 RCSwitch");

  initMQTT();
  Serial.println("Step 8 MQTT");

}

void loop() {
   
   filtrP = 100-(100*Summ1 / filtrV.toInt());
  
    

  FreeHeap = ESP.getFreeHeap();
      
  filtrVreal = Summ1;
  HTTP.handleClient();
  delay(1);

    if ((digitalRead(pinIn)== LOW )&&( flag1 ==false))  
  {
    flag1=true;
    Summ1=Summ1+vodomerIn.toInt();  

    Serial.printf("filtrP  %u\n", filtrP);
    if (filtrV.toInt()<Summ1) allarm(filtrV, String(Summ1));
    else 
    {
    Status = "Ok";
//    publishMQTT(pubTopic+mqttUser+"/Status", Status);
    }
    
    millis_ad1 = millis();

    Serial.printf("port LOW, Summ =  %u\n", Summ1);
    saveConfigSetup();
    publishMQTT(pubTopic+mqttUser+"/filtrVreal", filtrVreal);
    publishMQTT(pubTopic+mqttUser+"/filtrP", String(filtrP));


       lcd.home();                // At column=0, row=0
   lcd.print("OCTATOK: "+String(filtrP)+"%            ");   
   lcd.setCursor(0, 1);
   if (filtrP<1) 
     {
      lcd.print("3AMEHNTE filtr");
     }
    else 
    {
     for (int i=0; i <= filtrP/6.25; i++){
      lcd.print(char(255));
     }
      lcd.print("                          ");
    }
  }
 
  if ((digitalRead(pinIn)== HIGH )&&(flag1==true)&&(ad < millis()- millis_ad1))   flag1=false;
  
}
