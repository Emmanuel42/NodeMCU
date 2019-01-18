#include <LiquidCrystal_I2C.h>

#include <Arduino.h>
#include <ThingSpeak.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <DHT.h>

#include "secrets.h"

LiquidCrystal_I2C lcd(0x27,16,2);

// Préparation du capteur DHT
#define DHTPIN D4                      // broche sur laquelle est raccordée la data du capteur (la broche 'D4' du NodeMcu correspond à la broche 2 de l'arduino)
#define DHTTYPE DHT11                 // précise la référence du capteur DHT (DHT11 ou DHT21 ou DHT22)
DHT dht(DHTPIN, DHTTYPE);             // Initialisation du capteur DHT

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;

String apiKey = "Your API of thingsspeak";     //  Enter your Write API key from ThingSpeak
const char *ssid =  "Your wifi Network name";     // replace with your wifi ssid and wpa2 key
const char *pass =  "Network password";
const char* server = "api.thingspeak.com";

int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;



// fonction de démarrage

void setup() {

  pinMode(D7, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D5, OUTPUT);
  
  // Démarrage du bus série
 // Serial.begin(115200);               // vitesse
  lcd.init();                   
  lcd.backlight();
  lcd.setCursor(1,0);
  lcd.print("Temp. :");
  lcd.setCursor(14,0);
  lcd.print("*C");
  lcd.setCursor(1,1);
  lcd.print("Humi. :");
  lcd.setCursor(15,1);
  lcd.print("%");
    // Démarrage du capteur DHT11
  dht.begin();


  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);}

     WiFi.mode(WIFI_STA); 
/*  ThingSpeak.begin(client);*/

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(SECRET_SSID, SECRET_PASS);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }
}




// boucle infinie
void loop() {
  delay(20000);                        // attendre 20 secondes

  float t = dht.readTemperature();    // mesurer la température (en ° Celsius)
  float h = dht.readHumidity();

  if (isnan(h) || isnan(t)) 
                 {
                     Serial.println("Failed to read from DHT sensor!");
                      return;
                 }

                         if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
                      {  

                              HTTPClient http;
  
                              http.begin("http://51.75.126.33/DHT11/store_temp.php"); //HTTP
                               
                              http.addHeader("Content-Type", "application/json");
                              
                              String data = "{\"temperature\":\"" + String(t) + "\",\"humidite\":\"" + String(h) + "\"}";
                              Serial.println(data);
                              int httpCode = http.POST(data);

                              http.end();
                             /*
                             String postStr = apiKey;
                             postStr +="&field1=";
                             postStr += String(t);
                             postStr +="&field2=";
                             postStr += String(h);
                             postStr += "\r\n\r\n";
 
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);
                             */
 
                             Serial.print("Temperature: ");
                             Serial.print(t);
                             Serial.print(" degrees Celcius, Humidity: ");
                             Serial.print(h);
                             Serial.println("%. Send to VPS.");
                        }
          client.stop();
 
          Serial.println("Waiting...");
  
  // thingspeak needs minimum 15 sec delay between updates, i've set it to 30 seconds
  delay(20000);
  
  
  lcd.setCursor(9,0);
  
  lcd.print(t);
  
  lcd.setCursor(9,1);
  
  lcd.print(h);
  
  Serial.print(".");
  /*
  if (t >= 20.0){
    
    digitalWrite(D7, HIGH);
    digitalWrite(D6, LOW);
    digitalWrite(D5, LOW);
  }

    else if (t <= 17.0){
      digitalWrite(D7, LOW);
      digitalWrite(D6, LOW);
      digitalWrite(D5, HIGH);
    }
    else {
      digitalWrite(D7, LOW);
      digitalWrite(D6, HIGH);
      digitalWrite(D5, LOW);
  }
  */
  
  /*if ((WiFiMulti.run() == WL_CONNECTED)) {

    HTTPClient http;

    USE_SERIAL.print("[HTTP] begin...\n");
    // configure traged server and url
    //http.begin("https://192.168.1.12/test.html", "7a 9c f4 db 40 d3 62 5a 6e 21 bc 5c cc 66 c8 3e a1 45 59 38"); //HTTPS
    http.begin("http://51.75.126.33/DHT11"); //HTTP

    USE_SERIAL.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        USE_SERIAL.println(payload);
      }
    } else {
      USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }*/

  //delay(3000);


/*
  // set the fields with the values
  ThingSpeak.setField(1, t);
  ThingSpeak.setField(2, h);

 
  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(SECRET_CH_ID, SECRET_WRITE_APIKEY);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
 */
} 
