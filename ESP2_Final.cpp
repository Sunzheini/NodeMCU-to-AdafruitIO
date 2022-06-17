#include <LiquidCrystal_I2C.h>
#include <SimpleDHT.h>

#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "Maymuna"  // replace it with your Wifi Name
#define WLAN_PASS       ".............."  // Replace it with your wifi password

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "Sunzheini"  // Change this to your username
#define AIO_KEY         "............................."  // Change this to your key

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int buttonPin = 16;   
const int ledPin1 =  0;      
const int ledPin2 =  2;     
const int ledPin3 =  14;     
const int relayPin =  12;      

int pinDHT11 = 13;

SimpleDHT11 dht11(pinDHT11);
int sensorvalue2 = 0; 
int sensorvoltage2 = 0; 
float temperatureDHT11 = 0;
float humidityDHT11 = 0;

/************ Global State (you don't need to change this!) ******************/

WiFiClient client;
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

Adafruit_MQTT_Publish esp2_data = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/esp2data");
Adafruit_MQTT_Subscribe esp2_button = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/esp2button");

/*************************** Sketch Code ************************************/

void MQTT_connect();

void setup() {

  lcd.begin(16,2);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(5, 0);
  lcd.print("HELLO");
  lcd.setCursor(5, 1);      
  lcd.print("...");

  Serial.begin(115200);
  Serial.println("Hello");

  WiFi.begin(WLAN_SSID, WLAN_PASS);

  mqtt.subscribe(&esp2_button);

  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, LOW);
  digitalWrite(ledPin3, LOW);
  digitalWrite(relayPin, HIGH);

}

void loop() {

  MQTT_connect();
  
  Adafruit_MQTT_Subscribe *subscription;
  subscription = mqtt.readSubscription(5000);
  subscription == &esp2_button;

  if(strcmp((char *)esp2_button.lastread, "ON") == 0){
    digitalWrite(ledPin1, HIGH);
    digitalWrite(ledPin2, HIGH);
    digitalWrite(ledPin3, HIGH);
    digitalWrite(relayPin, LOW);
   
    delay(100);
    
   }
   if(strcmp((char *)esp2_button.lastread, "OFF") == 0){
    digitalWrite(ledPin1, LOW);
    digitalWrite(ledPin2, LOW);
    digitalWrite(ledPin3, LOW);
    digitalWrite(relayPin, HIGH);

    delay(100);
   }


    int err = SimpleDHTErrSuccess;
    if ((err = dht11.read2(&temperatureDHT11, &humidityDHT11, NULL)) != SimpleDHTErrSuccess) 
    {
      Serial.print("Read DHT11 failed, err="); Serial.println(err); delay(2000);
      return;
    }
    Serial.print("DHT11 Measurement: ");
    Serial.print(temperatureDHT11); Serial.print(" *C, ");
    Serial.print(humidityDHT11); Serial.println(" RH%"); 

    esp2_data.publish(temperatureDHT11);

    lcd.setCursor(5, 0);
    lcd.print(temperatureDHT11);
    lcd.print(" *C, ");
    lcd.setCursor(5, 1);      
    lcd.print(humidityDHT11);
    lcd.print(" RH%");

    delay(1000);
}

void MQTT_connect() 
{
  unsigned char ret;

  // Stop if already connected.
  if (mqtt.connected()) 
  {
    return;
  }
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) 
  { // connect will return 0 for connected
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) 
       {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
}