
// ks code for DHT 11 sensor with LCD screen display & live logging datas into a SD card

#include "DHT.h"
#include <SD.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h> // includes the LiquidCrystal_I2C Library
#include <virtuabotixRTC.h> // includes the library for RTC DS1302
#define DHTPIN 2     // what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 11

File myFile;
int pinCS = 53; // pin 10 on Arduino Uno !


// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);
// Creation of the display
LiquidCrystal_I2C lcd(0x27,20,4);
// Wiring infos for RTC
virtuabotixRTC myRTC(6, 7, 8);
//Icons on the LCD
byte th[8] = //icon for termometer
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

byte smiley[8] = {
  B00000,
  B10001,
  B00000,
  B00000,
  B10001,
  B01110,
  B00000,
};

byte deg[8] = {
  B00100,
  B01010,
  B00100,
  B00000,
  B00000,
  B00000,
  B00000,
};
void setup() {

  Serial.begin(9600);
  Serial.println("DHT11 test!");
  dht.begin();

  pinMode(pinCS, OUTPUT);
  // SD Card Initialization
  if (SD.begin()) {
    Serial.println("SD card is ready to use.");
  } else {
    Serial.println("SD card initialization failed");
    return;
  }

  lcd.init(); // Initializes the interface to the LCD screen
  lcd.backlight();  // Enable the backlight for better visibility

  lcd.createChar(1,th);
  lcd.createChar(2,hum);
  lcd.createChar(3,smiley);
  lcd.createChar(4,deg);

  //Set current date : (sec,min,hours,day,daydate,monthdate,yeardate)
 // myRTC.setDS1302Time(40,20,11,3,13,6,2018);
  // Have to be commented after first programmation. 
  // We don't want to reprogram the date for each use, that's why we use battery
}

void loop() {
  
  // Wait a few seconds between measurements.
  delay(1000);
  
  myRTC.updateTime(); //Updating the time from RTC
  
  // Reading temperature or humidity takes about 250 milliseconds!
  int h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  int t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }


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
  Serial.print(" *C\n ");

  // Writing datas inside SD file
  myFile = SD.open("dht11.txt", FILE_WRITE);
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
    myFile.println(" *C\n ");
    myFile.close();
  }
  // if the file didn't open, print an error:
  else {
    Serial.println("error opening dht11.txt");
  }


  // Printing part on the LCD Screen



  lcd.setCursor(0,0);
  lcd.print(myRTC.dayofmonth);
  lcd.print("/");
  lcd.print(myRTC.month);
  lcd.print("/");
  lcd.print(myRTC.year);
  lcd.print("  ");
  lcd.print(myRTC.hours);
  lcd.print(":");
  lcd.print(myRTC.minutes);
  lcd.setCursor(1,1);
  lcd.write(1);
  lcd.print(t); // Prints the temperature value from the sensor
  lcd.write(4);
  lcd.print("C  ");
  lcd.write(3);
  lcd.print("  ");
  lcd.write(2);
  lcd.print(h);
  lcd.print("%");
}



