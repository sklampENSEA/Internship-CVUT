/*
  
Code for DHT 11 sensor displayed throught wifi on an mqtt server, hosted directly on the home assistant server.

  http://192.168.0.101:8123/states to connect home assistant
  
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"          // Librairies for DHT

#define wifi_ssid "Intership"           // Here is where you put the network name
#define wifi_password "42788201"        // Here is the password, always keep the "" for both

#define mqtt_server "192.168.0.101"  // Mqtt ip adress, same as the smarthome ip adress in our case
//#define mqtt_user "homeassistant"      // if exist
//#define mqtt_password "admin"  // but not in our case for now

#define temperature_topic "sensor/temperature"  //Topic temperature
#define humidity_topic "sensor/humidity"        //Topic humidity

//Buffer to decode MQTT messages
char message_buff[100];

long lastMsg = 0;   
long lastRecu = 0;
bool debug = false;  //Display log message if True

#define DHTPIN D4    // DHT Pin 
#define DHTTYPE DHT11       // DHT 11 

// Create abjects
DHT dht(DHTPIN, DHTTYPE);     
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);     
  pinMode(D2,OUTPUT);     //Pin 2 for LED
  setup_wifi();           //Connect to Wifi network
  client.setServer(mqtt_server, 1883);    // Configure MQTT connexion
  client.setCallback(callback);           // callback function to execute when a MQTT message   
  dht.begin();
}

//Connexion au réseau WiFi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi OK ");
  Serial.print("=> ESP8266 IP address: ");
  Serial.print(WiFi.localIP());
}

//Reconnexion
void reconnect() {

  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker ...");
    if (client.connect("ESP8266Client")) {
      Serial.println("OK");
    } else {
      Serial.print("KO, error : ");
      Serial.print(client.state());
      Serial.println(" Wait 5 secondes before to retry");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  // Send a message every seconds (unit is in milliseconds right below)
  if (now - lastMsg > 1000) {
    lastMsg = now;
    // Read humidity
    float h = dht.readHumidity();
    // Read temperature in Celcius
    float t = dht.readTemperature();

    // Oh, nothing to send
    if ( isnan(t) || isnan(h)) {
      Serial.println("KO, Please check DHT sensor !");
      return;
    }
  
    if ( debug ) { // Print in the serial monitor if debug = true
      Serial.print("Temperature : ");
      Serial.print(t);
      Serial.print(" | Humidity : ");
      Serial.println(h);
    }  
    client.publish(temperature_topic, String(t).c_str(), true);   // Publish temperature on temperature_topic
    client.publish(humidity_topic, String(h).c_str(), true);      // and humidity on mqtt SmartHome
  }
  if (now - lastRecu > 100 ) { // Time between two payload fetching, in milliseconds
    lastRecu = now;
    client.subscribe("homeassistant/switch1");
  }
}

// MQTT callback function

void callback(char* topic, byte* payload, unsigned int length) {

  int i = 0;
  if ( debug ) { // Print in the serial monitor if debug = true
    Serial.println("Message recu =>  topic: " + String(topic));
    Serial.print(" | longueur: " + String(length,DEC));
  }
  // create character buffer with ending null terminator (string)
  for(i=0; i<length; i++) {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';
  
  String msgString = String(message_buff);
  if ( debug ) {
    Serial.println("Payload: " + msgString);
  }
  
  if ( msgString == "ON" ) {
    digitalWrite(D2,HIGH);  
  } else {
    digitalWrite(D2,LOW);  
  }
}
