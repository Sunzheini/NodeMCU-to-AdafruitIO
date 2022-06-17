#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "Maymuna"  // replace it with your Wifi Name
#define WLAN_PASS       "........."  // Replace it with your wifi password

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "Sunzheini"  // Change this to your username
#define AIO_KEY         ".............................."  // Change this to your key

const int inputPin =  4;      // D2
const int relayPin =  0;      // D3

/************ Global State (you don't need to change this!) ******************/

WiFiClient client;
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

Adafruit_MQTT_Publish esp1_data = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/esp1data");
Adafruit_MQTT_Subscribe esp1_button = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/esp1button");

/*************************** Sketch Code ************************************/

void MQTT_connect();

void setup() {

  WiFi.begin(WLAN_SSID, WLAN_PASS);

  mqtt.subscribe(&esp1_button);

  pinMode(inputPin, INPUT);
  pinMode(relayPin, OUTPUT);

  digitalWrite(relayPin, HIGH);
}

void loop() {

  MQTT_connect();
  
  Adafruit_MQTT_Subscribe *subscription;
  subscription = mqtt.readSubscription(5000);
  subscription == &esp1_button;

  if(strcmp((char *)esp1_button.lastread, "ON") == 0){
    digitalWrite(relayPin, LOW);
   
    delay(100);
   }
   
   if(strcmp((char *)esp1_button.lastread, "OFF") == 0){
    digitalWrite(relayPin, HIGH);

    delay(100);
   }

    if(inputPin == HIGH){
      esp1_data.publish(0);
    }
    else{
      esp1_data.publish(1);
    }

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