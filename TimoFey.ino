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

#include <ESP8266httpUpdate.h>
#include <DNSServer.h>
  

LiquidCrystal_I2C lcd(0x27,16,2); // Check I2C address of LCD, normally 0x27 or 0x3F

byte bukva_B[8]   = {B11110,B10000,B10000,B11110,B10001,B10001,B11110,B00000,}; // Буква "Б"
byte bukva_G[8]   = {B11111,B10001,B10000,B10000,B10000,B10000,B10000,B00000,}; // Буква "Г"
byte bukva_D[8]   = {B01111,B00101,B00101,B01001,B10001,B11111,B10001,B00000,}; // Буква "Д"
byte bukva_ZH[8]  = {B10101,B10101,B10101,B11111,B10101,B10101,B10101,B00000,}; // Буква "Ж"
byte bukva_Z[8]   = {B01110,B10001,B00001,B00010,B00001,B10001,B01110,B00000,}; // Буква "З"
byte bukva_I[8]   = {B10001,B10011,B10011,B10101,B11001,B11001,B10001,B00000,}; // Буква "И"
byte bukva_IY[8]  = {B01110,B00000,B10001,B10011,B10101,B11001,B10001,B00000,}; // Буква "Й"
byte bukva_L[8]   = {B00011,B00111,B00101,B00101,B01101,B01001,B11001,B00000,}; // Буква "Л"
byte bukva_P[8]   = {B11111,B10001,B10001,B10001,B10001,B10001,B10001,B00000,}; // Буква "П"
byte bukva_Y[8]   = {B10001,B10001,B10001,B01010,B00100,B01000,B10000,B00000,}; // Буква "У"
byte bukva_F[8]   = {B00100,B11111,B10101,B10101,B11111,B00100,B00100,B00000,}; // Буква "Ф"
byte bukva_TS[8]  = {B10010,B10010,B10010,B10010,B10010,B10010,B11111,B00001,}; // Буква "Ц"
byte bukva_CH[8]  = {B10001,B10001,B10001,B01111,B00001,B00001,B00001,B00000,}; // Буква "Ч"
byte bukva_Sh[8]  = {B10101,B10101,B10101,B10101,B10101,B10101,B11111,B00000,}; // Буква "Ш"
byte bukva_Shch[8]= {B10101,B10101,B10101,B10101,B10101,B10101,B11111,B00001,}; // Буква "Щ"
byte bukva_Mz[8]  = {B10000,B10000,B10000,B11110,B10001,B10001,B11110,B00000,}; // Буква "Ь"
byte bukva_IYI[8] = {B10001,B10001,B10001,B11001,B10101,B10101,B11001,B00000,}; // Буква "Ы"
byte bukva_Yu[8]  = {B10010,B10101,B10101,B11101,B10101,B10101,B10010,B00000,}; // Буква "Ю"
byte bukva_Ya[8]  = {B01111,B10001,B10001,B01111,B00101,B01001,B10001,B00000,}; // Буква "Я"


  
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
String bin = "http://www.wk-ufa.ru/TimoFey.ino.bin";
  
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

String filtrV = "1";       // Ресурс фильтра в литрах
String filtrT = "1";       // Ресурс фильтра в днях 
String filtrTdata = "01.01.3000";       // Дата установки дд.мм.гггг
String filtrVreal = "1";       // Использовано литров 
String vodomerIn = "1";       // Коэффициент литров/импульс вход 
String vodomerOut = "1";       // Коэффициент литров/импульс слив 

String Status = "Ok";

long Year_i = 2018;
long Month_i = 01;
long Day_i = 01;
long now_i = 01;
int ost_i = 0;

  
String  mqttServer = "";   // 77.79.191.226
String  mqttPort =  "";     // 
String  mqttUser =  "";     // 
String  mqttPass =  "";     // 

String pubTopic = "SMARTHOUSE/";       //smart_house/<номер_договора>/<код_устройства>/<статус_устройства>​ . Например: smart_house/72357995ASU/Door1/OPEN
String cTopic = "";       // состояние геркона для публикации, например OPEN
String controlTopic = "SMARTHOUSE/#";

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


bool flag = false;        // Флаг что импульс посчитан
long Summ1 = 0;            // Количество импульсов
int pinIn = 0;           // Порт импульсного входа gpio0
int ad = 200;           // Время для проверки антидребезга в мс
long millis_ad1 = 0;     // переменная для хранения millis() проверки антидребезга
long millis_wifi = 0;     // переменная для хранения millis() wifi подключения

String pr="";

int pinOut = 1; 

int filtrP = 0; 


int pinrele = 5; 
int pinled = 2; 

 
void setup() {

    Serial.begin(115200);

    lcd.begin(4,5);      // In ESP8266-01, SDA=0, SCL=2               
    lcd.backlight();
    lcd.clear();
    lcd.createChar(0, bukva_I);  // создаем символы и записываем их в память LCD
    lcd.createChar(1, bukva_F);  
    lcd.createChar(2, bukva_L);
    lcd.createChar(3, bukva_Mz);    
    
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
 
  Serial.println("Step 7 MQTT");
  initMQTT();
  
  filtr();
  Led();      //Функция отрисовки экрана

}

void loop() {
   
  FreeHeap = ESP.getFreeHeap();
  HTTP.handleClient();
  delay(1);
  filtr();
  
}
