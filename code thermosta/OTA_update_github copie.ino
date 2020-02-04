#include <SPI.h>
#include <Wire.h>
#include "Adafruit_SSD1306.h"
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266httpUpdate.h>
#include <EEPROM.h>


#define OLED_RESET 0
Adafruit_SSD1306 display(OLED_RESET);
#define DHTPIN D5
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

int update_boot=1;
unsigned long previousMillis = 0;
const long interval = 60000;// tchek delay version

String ssid;
String mdp;


//////////////////////////////////////UPDATE_FIRMWARE/////////////////////////////////////////////////////////////////////////////////////////////////////////

void FirmwareUpdate()//OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
{
 Serial.println("recherche de mise à jour");
 HTTPClient http;
 const String Firmware_Version={"1.0"};

 #define URL_firmware_Version "https://raw.githubusercontent.com/gauix4/thermosta/master/Firmware_Version.txt"
 #define URL_firmware_Bin "https://raw.githubusercontent.com/gauix4/thermosta/master/firmware.bin"

  http.begin(URL_firmware_Version,"CC AA 48 48 66 46 0E 91 53 2C 9C 7C 23 2A B1 74 4D 29 9D 33");
  delay(100);
  int httpCode = http.GET();
  delay(100);
  String http_version;

  if (httpCode == HTTP_CODE_OK)
  {
    http_version = http.getString();
    Serial.println(http_version);
  }
  else
  {
    Serial.println("Erreur verification firmware:");
  }
  http.end();


 if (httpCode == HTTP_CODE_OK)
 {

    if(http_version<(Firmware_Version))
    {
     Serial.println("Version du firmware déjà à jour ok");
    }

    if(http_version.equals(Firmware_Version) )
    { Serial.println("ok");
     Serial.println("Version du firmware indentique");
    }

    if(http_version>(Firmware_Version))
    {
     Serial.println("Nouvel mise à jour du firmware disponible");
     WiFiClient client;
     delay(5000);
     Serial.println("mise à jour du firmware");
     ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);


     t_httpUpdate_return ret = ESPhttpUpdate.update(URL_firmware_Bin,"","CC AA 48 48 66 46 0E 91 53 2C 9C 7C 23 2A B1 74 4D 29 9D 33");

      switch (ret)
      {
       case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_MISE_A_JOUR_ECHOUER_ERREUR (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

       case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_MISE_A_JOUR_NON_EFFECTUEE");
        break;

       case HTTP_UPDATE_OK:
        Serial.println("HTTP_MISE_A_JOUR_REUSSI_!!!");
        break;
      }
    }
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void repeatedCall()
{
 unsigned long currentMillis = millis();

  if ((currentMillis - previousMillis) >= interval)
  {
    previousMillis = currentMillis;
    FirmwareUpdate();
  }
}


void capteur()//OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  float bat = ESP.getVcc();// / 1024;
  t=t+00.5;
  delay(100);

  display.clearDisplay();
  display.display();
  display.setTextSize(4,5);
  display.setTextColor(WHITE);
  display.setCursor(18,14);
  display.print(t,1);
  display.display();
}


void On()  //OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
{ Serial.println("ON");

 display.setTextSize(4,5);
 display.setTextColor(WHITE);
 display.setCursor(104,34);
 display.print(".");
 display.display();

  WiFiClient client;
  //int var=0;
  if (!client.connect("192.168.4.1",80))
  {
   delay(10);
   return;
   }



 client.print("192,168,4,1/on\r");
 client.flush();
 client.stop();
 delay(10);



 //sleep();
}


void Off()  //OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
{  Serial.println("OFF");

 display.setTextSize(4,5);
 display.setTextColor(WHITE);
 display.setCursor(104,34);
 display.print(" ");
 display.display();

  WiFiClient client;
  //int var=0;
  if (!client.connect("192.168.4.1",80) )
  {
     delay(10);
     return;
   }



 client.print("192,168,4,1/off\r");
 client.flush();
 client.stop();
 delay(5000);


 //sleep();
}



void sleep() //OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
{
 //ESP.deepSleep(90000000);
  //ESP.deepSleep(5000000);
  //setup();
}

  String save(){
  Serial.println("taille eeprom "+String(EEPROM.length()));
  ssid = WiFi.SSID();
  mdp = WiFi.psk();
  Serial.println(ssid);
  Serial.println(mdp);
  ssid != WiFi.SSID();
  mdp != WiFi.psk();
  Serial.println(ssid);
  Serial.println(mdp);
  EEPROM.put(0,(ssid));
  EEPROM.put(1,(mdp));
  EEPROM.commit();
  EEPROM.end(); // pour liberer la memoire
  Serial.println("Enregistrement memoire ok");
}


void connect_internet()
{
  WiFiManager wifiManager;
  wifiManager.autoConnect("connecte moi au wifi ! ;)");

}


void setup() {
  EEPROM.begin(128);
  dht.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  Serial.begin(115200);
  display.setRotation(2);
  pinMode(A0,INPUT);
  pinMode(D6,INPUT_PULLUP);


  connect_internet();
  On();
  FirmwareUpdate();
  Off();
  On();
  FirmwareUpdate();

}
void loop()
{




 int a = analogRead(A0);
 int b = digitalRead(D6);



if (a >= 200)
{

 //connect_wifi();
 //Off();
}

if (b >= 1)
{
 //connect_wifi();
 //On();
}

}



//if (update_boot==1)
//{
//  update_boot=0;
//  FirmwareUpdate();
//}
