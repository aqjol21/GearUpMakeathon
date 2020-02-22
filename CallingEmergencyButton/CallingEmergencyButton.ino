#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>


String message;
const int latAddress = 10; 
const int lonAddress = 50;
const int buttonPin = 12;
int buttonState = 0;  
int GPSBaud = 9600;
TinyGPSPlus gps;
SoftwareSerial mySerial(7, 8); //SIM900 Tx & Rx is connected to Arduino #7 & #8
SoftwareSerial gpsSerial(2, 3);
float currentLattitude;
float currentLongitude;
float la;
float lo;

void checkPrecision(){
  la = EEPROM.get(latAddress, la); //previous value
  lo = EEPROM.get(lonAddress, lo);

  message = "Emergency ! My location: https://www.google.com/maps?q=" +String(la,6) + "," + String(lo,6);
 // EEPROM.put(latAddress, la);
 //  EEPROM.put(lonAddress, lo);
  Serial.println(message);
  Serial.println(EEPROM.get(latAddress, la),6);
  Serial.println(EEPROM.get(lonAddress, lo),6);
  if(abs((currentLattitude-la)*1000000)>=70){
     la = currentLattitude;
     EEPROM.put(latAddress, la);
  }
  
  if(abs((currentLongitude-lo)*1000000)>=70){
     lo = currentLongitude;
     EEPROM.put(lonAddress, lo);
  }
  Serial.println(EEPROM.get(latAddress, la),6);
  Serial.println(EEPROM.get(lonAddress, lo),6);
  message = "Emergency ! My location: https://www.google.com/maps?q=" +String(la,6) + "," + String(lo,6);

}

void setup()
{
  // Start the Arduino hardware serial port at 9600 baud
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
  currentLattitude = EEPROM.get(latAddress, la);;
  currentLongitude = EEPROM.get(lonAddress, lo) ;
  message = "Emergency ! My location: https://www.google.com/maps?q=" + String(currentLattitude,6) + "," + String(currentLongitude,6);
  // Start the software serial port at the GPS's default baud
  gpsSerial.begin(GPSBaud);
}

void loop()
{
  // This sketch displays information every time a new sentence is correctly encoded.
  while (gpsSerial.available() > 0)
    if (gps.encode(gpsSerial.read()))
      displayInfo();

  // If 5000 milliseconds pass and there are no characters coming in
  // over the software serial port, show a "No GPS detected" error
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println("No GPS detected");
    buttonState = digitalRead(buttonPin);

   if (buttonState == HIGH) {
    gpsSerial.end();
    mySerial.begin(9600);  
    sendSms();
    gpsSerial.begin(GPSBaud);

  } 
    while(true);
  }
    buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH) {
    gpsSerial.end();
    mySerial.begin(9600);  
    sendSms();
      gpsSerial.begin(GPSBaud);

  } 
}

void displayInfo()
{
  if (gps.location.isValid())
  {
    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(), 6);
    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);
    Serial.print("Altitude: ");
    Serial.println(gps.altitude.meters());
    currentLattitude = float(gps.location.lat(),6);
    currentLongitude = float(gps.location.lng(),6);
    //message = String(currentLattitude,6) + ", " + String(currentLongitude,6); //?String
    checkPrecision();
  }
  else
  {
    Serial.println("Location: Not Available");
  }
  
  Serial.print("Date: ");
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    //message = gps.date.month();
    Serial.print("/");
    Serial.print(gps.date.day());
    Serial.print("/");
    Serial.println(gps.date.year());
  }
  else
  {
    Serial.println("Not Available");
  }

  Serial.print("Time: ");
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(":");
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(":");
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(".");
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.println(gps.time.centisecond());
  }
  else
  {
    Serial.println("Not Available");
  }
  
  Serial.println(message);
  buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH) {
    gpsSerial.end();
    mySerial.begin(9600);  
    sendSms();
      
      gpsSerial.begin(GPSBaud);
      displayInfo();

  } 
  
  Serial.println();
  delay(1000);
}

void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}


void SIM900power()
{
  pinMode(9, OUTPUT); 
  digitalWrite(9,LOW);
  delay(1000);
  digitalWrite(9,HIGH);
  delay(1500);
  digitalWrite(9,LOW);
}

void sendSms(){
  
  Serial.println("Initializing..."); 
  SIM900power();
  delay(8000);

  mySerial.println("AT"); //Handshaking with SIM900
  updateSerial();

  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
   Serial.println(Serial.read());
 
  mySerial.println("AT+CMGS=\"+77771417099\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms 77771417099
  updateSerial();
  //Serial.println("messa");
  mySerial.print(message); //text content
  updateSerial();
  mySerial.write(26);
  delay(3000);
  SIM900power();
  delay(2000);
  call();
  mySerial.end();
}
void call(){
  SIM900power();
  Serial.println("Initializing..."); 
  delay(7000);

  mySerial.println("AT"); //Handshaking with SIM900
  updateSerial();
  
  mySerial.println("ATD+ +77771417099;"); //  change ZZ with country code and xxxxxxxxxxx with phone number to dial
  updateSerial();
  delay(10000); // wait for 10 seconds...
  mySerial.println("ATH"); //hang up
  updateSerial();
  SIM900power();
  delay(2000);
}
