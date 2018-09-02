
// Ks code for DHT 11 sensor with the timestamp.
// Provides a data registering onto a SD card, a display throughout the serial monitor,
// an LCD display and wifi ( requires blynk app + the auth[] key writen below)

#include "DHT.h"
#include <SD.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h> // includes the LiquidCrystal_I2C Library
#include <virtuabotixRTC.h> // includes the library for RTC DS1302
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#define BLYNK_PRINT Serial

#define DHTPIN 2     // what digital pin we're connected to

#define DHTTYPE DHT11   // DHT 11

#define EspSerial Serial1 // Hardware Serial on Mega, Leonardo, Micro...
// or Software Serial on Uno, Nano...
//#include <SoftwareSerial.h>
//SoftwareSerial EspSerial(2, 3); // RX, TX
// Using this code, we can emulate other RX & TX pins
// for serial communication. It is required for Arduino Uno
// since the RX0 & TX0 are already used by the USB connexion
// to the computer, and therefore they are not available.

// Your ESP8266 baud rate:
#define ESP8266_BAUD 115200 // Depends on the ESP module, ours is 115200

// Initialize Wifi communication
ESP8266 wifi(&EspSerial);

// Initialize SD file
File myFile;

// Indicate the pin where we plug the RTC module
int pinCS = 53; // pin 10 on Arduino Uno & 53 on Mega !


// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);
// Creation of the display
LiquidCrystal_I2C lcd(0x27,20,4);
// Wiring infos for RTC
virtuabotixRTC myRTC(6, 7, 8);
//Icons on the LCD
byte th[8] = //icon for thermometer
{
    B00100,
    B01010,
    B01010,
    B01110,
    B01110,
    B11111,
    B11111,
    B01110
};

byte hum[8] = //icon for water droplet
{
    B00100,
    B00100,
    B01010,
    B01010,
    B10001,
    B10001,
    B10001,
    B01110,
};

byte smiley[8] = { // icon for smiley
  B00000,
  B10001,
  B00000,
  B00000,
  B10001,
  B01110,
  B00000,
};

byte deg[8] = { //icon for degrees = °
  B00100,
  B01010,
  B00100,
  B00000,
  B00000,
  B00000,
  B00000,
};


// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "e9a83c1de755439997fa31c65982d722";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Intership"; // Always keep the "" !
char pass[] = "42788201"; // Here aswell


void setup()
{

  // Set ESP8266 baud rate
  EspSerial.begin(ESP8266_BAUD);

// Open the serial communication
  Serial.begin(9600);
  Serial.println("DHT11 test!");

// Start to connect to wifi
  Serial.println("Connecting to Wifi ...");
  Blynk.begin(auth, wifi, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, wifi, ssid, pass, "blynk-cloud.com", 8442);
  //Blynk.begin(auth, wifi, ssid, pass, IPAddress(192,168,1,100), 8442);
  
  dht.begin(); // Initialize the dht
  Serial.println("Wifi connected !");
  Serial.println("Start SD Card initialization ...");
  pinMode(pinCS, OUTPUT); // Initialize the RTC module
  
  // SD Card Initialization
  if (SD.begin()) {
    Serial.println("SD Card is ready to use.");
  } else {
    Serial.println("SD Card initialization failed");
    return;
  }

  lcd.init(); // Initializes the interface to the LCD screen
  lcd.backlight();  // Enable the backlight for better visibility

  lcd.createChar(1,th); // Create all icons that we set previously
  lcd.createChar(2,hum); // and attribute them a number 
  lcd.createChar(3,smiley);
  lcd.createChar(4,deg);

  //Set current date : (sec,min,hours,day,daydate,monthdate,yeardate)
 // myRTC.setDS1302Time(40,20,11,3,13,6,2018);
  // Have to be commented after first programmation. 
  // We don't want to reprogram the date for each use, that's why we use battery
  // and we then comment this part after first use
}

void loop()
{
  Blynk.run();
  // Wait a few seconds between measurements.
  delay(1000); // Curently 1s but can change the value (on milliseconds here)
  
  int h = dht.readHumidity();
  int t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V5, h);
  Blynk.virtualWrite(V6, t);
  
  myRTC.updateTime(); // Updating the time from RTC

 //  Print everything in the serial monitor
  Serial.print(myRTC.dayofmonth);
  Serial.print("/");
  Serial.print(myRTC.month);
  Serial.print("/");
  Serial.print(myRTC.year);
  Serial.print("  ");
  Serial.print(myRTC.hours);
  Serial.print(":");
  Serial.print(myRTC.minutes);
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C \n ");

  // Writing datas inside SD file
  myFile = SD.open("dht11.txt", FILE_WRITE); // can change the file name by modifying here "dht11.txt"
  // if the file opened okay, write to it:
  if (myFile) {
    myFile.print(myRTC.dayofmonth);
    myFile.print("/");
    myFile.print(myRTC.month);
    myFile.print("/");
    myFile.print(myRTC.year);
    myFile.print("  ");
    myFile.print(myRTC.hours);
    myFile.print(":");
    myFile.print(myRTC.minutes);
    myFile.print("  Humidity: ");
    myFile.print(h);
    myFile.print(" %\t");
    myFile.print("Temperature: ");
    myFile.print(t);
    myFile.println(" *C");
    myFile.close();
  }
  // if the file didn't open, print an error:
  else {
    Serial.println("error opening dht11.txt");
  }


  // Printing part on the LCD Screen
  lcd.setCursor(0,0); // set cursor line 1
  lcd.print(myRTC.dayofmonth);
  lcd.print("/");
  lcd.print(myRTC.month); // dates & time ..
  lcd.print("/");
  lcd.print(myRTC.year);
  lcd.print("  ");
  lcd.print(myRTC.hours);
  lcd.print(":");
  lcd.print(myRTC.minutes);
  lcd.setCursor(1,1); // set cursor line 2 a pixel ahead to get things centered
  lcd.write(1); // thermometer symbol
  lcd.print(t); // temperature value from the sensor
  lcd.write(4); // Degree symbol °
  lcd.print("C  ");
  lcd.write(3); // Smiley symbol
  lcd.print("  ");
  lcd.write(2); // Droplet symbol
  lcd.print(h); // humidity value from the sensor
  lcd.print("%");

}
