#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ESP8266httpUpdate.h>

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

unsigned long previousMillis=0 ;
unsigned long interval = 3600000;
int ok=0;



IPAddress ip(192,168,1,33);
WiFiServer server(80);

const byte LED_PIN = 2;
const int RELAY = 0;

void led_off(){
ok=0;
Serial.println("Led on");
digitalWrite(LED_PIN,HIGH);
digitalWrite(RELAY, HIGH);

}

void led_on(){
ok=1;
previousMillis = millis();
Serial.println("Led off");
digitalWrite(LED_PIN, LOW);
digitalWrite(RELAY, LOW);
}



// Affichage de la page d'accueil
// Code javascript pour appeler les fonctions on et off
// Tableau html pour les 2 boutons
// Bouton "On"  (html) --> appelle fonction "on()"  (javascript) --> envoie requete "192.168.4.1/on"
// Bouton "Off" (html) --> appelle fonction "off()" (javascript) --> envoie requete "192.168.4.1/off"
String homepage() {
  String s;
  s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html><body>\r\n";
  s +="<script language='javascript' type='text/javascript'>\r\n";
  s +="<!--\r\n";
  s +="function on()  { var xhr = new XMLHttpRequest(); xhr.open('GET', '/on',  true); xhr.send();}\r\n";
  s +="function off() { var xhr = new XMLHttpRequest(); xhr.open('GET', '/off', true); xhr.send();}\r\n";
  s +="//-->\r\n";
  s +="</script>\r\n";
  s +="<table align='center' style='cursor: pointer;'><tr>\r\n";
  s +="<td height='100' width='100' bgcolor='#f45942' align='center'><font size='20'><div onclick='off();'>Off</div></font></td>\r\n";
  s +="<td height='100' width='100' bgcolor='#41f492' align='center'><font size='20'><div onclick='on();'>On </div></font></td>\r\n";
  s += "</tr></table></body></html>\r\n";
  return s;
}

boolean matchcommand(String req,String command){
  if (req.indexOf(command) != -1){
    return true;
  } else {
    return false;
  }
}


void setup() {

  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY, OUTPUT);
  digitalWrite(LED_PIN,HIGH);
  digitalWrite(RELAY, HIGH);

  WiFiManager wifiManager;
  wifiManager.setSTAStaticIPConfig (IPAddress ( 192 , 168 , 0 , 33 ));
  wifiManager.autoConnect("je suis la chaudiere ");


  server.begin();

  Serial.println("HTTP server started");


}

void loop() {

if( ok==1 && millis() - previousMillis >= interval) {
    led_off();
    }

  // Vérifie si client se connecte
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  // Attends que le client envoie des données
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }

  // Affiche la page d'accueil 192.168.4.1
  client.print(homepage());

  // Lit la premiere ligne de la requete
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  if (matchcommand(req,"off")){
    // requête 192.168.4.1/off --> éteins led
    led_off();
  }
  else if (matchcommand(req,"on")){
    // requête 192.168.4.1/on --> allume led
    led_on();
  }

      else {
  Serial.println("invalid request");
  client.stop();
  return;
  }
  client.flush();

}
