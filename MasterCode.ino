/////////////////////////////////////////////////////////////////////////////////////////////////////////
//      This is the Main Flight Computer for the StratoScience portion of the HABGab 2023 Flight.      //
//      This controls the two of three onboard cutdown systems,                                        //
//      and the Ozone and Power generation experiments.                                                //
//                                                                                                     //
//      Contributers:                                                                                  //
//      Amish Chawla                                                                                   //
//      Tarun Kumar                                                                                    //
//      Seth Kendall                                                                                   //
//      Max Kendall                                                                                    //
//      Charlie Nicholson                                                                              //
//                                                                                                     //
//      and of course...                                                                               //
//      Mikal Hart for the excellent TinyGPSPlus library                                               //
//                                                                                                     //
//      Last Updated: July 13, 2023, version 2.0.7                                                     //
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//Libraries
#include <SPI.h>
#include <SD.h>
#include <NeoSWSerial.h>
#include <TinyGPSPlus.h>

//Pin Numbers
#define CUTDOWN_HELO 9
#define CUTDOWN_GLIDER 10
#define CUTDOWN_HELO_TOAPRS 7
#define CUTDOWN_GLIDER_TOAPRS 8
#define GPS_RX_PIN 6
#define GPS_TX_PIN 3
#define SD_CHIP_SELECT 4
#define UV_AIN A0
#define UV_AIN1 A1
#define WM_PIN A2

//Constants
#define SEALEVELPRESSURE_HPA 1013.25
#define VOLTAGE_DIVIDER 0.25
#define RESISTANCE_IN_OHMS 120  // 120 Ohms

//Objects
File dataFile;
TinyGPSPlus gps;
NeoSWSerial ss(GPS_RX_PIN, GPS_TX_PIN);

//Value Holders
int gpsalt;
bool hasCutGlider = 0;
bool hasCutHelo = 0;

void setup() {
  //Serial Communications
  Serial.begin(9600);
  ss.begin(38400);

  //Pin Modes
  pinMode(UV_AIN, INPUT);
  pinMode(UV_AIN1, INPUT);
  pinMode(CUTDOWN_HELO, OUTPUT);
  pinMode(CUTDOWN_GLIDER, OUTPUT);
  pinMode(CUTDOWN_HELO_TOAPRS, OUTPUT);
  pinMode(CUTDOWN_GLIDER_TOAPRS, OUTPUT);

  //Initilize Cutdown Systems
  digitalWrite(CUTDOWN_HELO, LOW);
  digitalWrite(CUTDOWN_HELO_TOAPRS, LOW);
  digitalWrite(CUTDOWN_GLIDER, LOW);
  digitalWrite(CUTDOWN_GLIDER_TOAPRS, LOW);

  if (!SD.begin(SD_CHIP_SELECT)) {
    Serial.println("Fix card");
    while (1)
      ;
  }
  Serial.println("Card initialized.");
  delay(1000);
}

void loop() {
  while (ss.available() > 0) {
    if (gps.encode(ss.read())) {
      if (gps.altitude.isValid()) {
        Serial.println("VALID!");
        gpsalt = gps.altitude.meters();
      } else {
        Serial.println("INVALID Altitude");
      }

      dataFile = SD.open("GPSdata.txt", FILE_WRITE);
      if (dataFile) {
        dataFile.print(gpsalt);
        dataFile.print(",");
        dataFile.print(gps.location.lat(), 6);
        dataFile.print(",");
        dataFile.println(gps.location.lng(), 6);
        dataFile.close();


        Serial.print("GPS Altitude: ");
        Serial.print(gpsalt);
        Serial.print(", Latitude: ");
        Serial.print(gps.location.lat(), 6);
        Serial.print(", Longitude: ");
        Serial.println(gps.location.lng(), 6);
      } else {
        Serial.print("Error opening ");
        Serial.println("GPSdata.txt");
      }

      //////CUT DOWN GLIDER
      if ((gpsalt >= 24384) && !hasCutGlider) {
        Serial.println("Detected Cutdown Altitude - Helo");
        delay(500);
        if (gps.altitude.isValid()) {
          gpsalt = gps.altitude.meters();
        }
        if (gpsalt >= 24384) {
          Serial.println("CUTTING DOWN GLIDER!");
          digitalWrite(CUTDOWN_GLIDER, HIGH);
          delay(5000);
          digitalWrite(CUTDOWN_GLIDER, LOW);
          Serial.println("GLIDER IS AWAY!");
          digitalWrite(CUTDOWN_GLIDER_TOAPRS, HIGH);
          hasCutGlider = 1;
        }
      }
      /////CUT DOWN HELO
      if ((gpsalt >= 24689) && !hasCutHelo) {
        Serial.println("Detected Cutdown Altitude - Helo");
        delay(500);
        if (gps.altitude.isValid()) {
          gpsalt = gps.altitude.meters();
        }
        if (gpsalt >= 24689) {
          Serial.println("CUTTING DOWN HELO!");
          digitalWrite(CUTDOWN_HELO, HIGH);
          delay(5000);
          digitalWrite(CUTDOWN_HELO, LOW);
          Serial.println("HELO IS AWAY!");
          digitalWrite(CUTDOWN_HELO_TOAPRS, HIGH);
          hasCutHelo = 1;
        }
      }

      recordData("UV0.txt", UV_AIN, "UV0: ");
      recordData("UV1.txt", UV_AIN1, "UV1: ");
      recordWindmillData("windmill.txt", WM_PIN, "WM: ");

      Serial.println("---------------------------------------------------------------");
      delay(250);
    }
  }
}

void recordData(const char* fileName, int pin, const char* message) {
  dataFile = SD.open(fileName, FILE_WRITE);
  if (dataFile) {
    int sensorValue = analogRead(pin);
    dataFile.println(sensorValue);
    dataFile.close();
  } else {
    Serial.print("Error opening ");
    Serial.println(fileName);
  }
  Serial.print(message);
  Serial.println(analogRead(pin));
  //delay(250);
}

void recordWindmillData(const char* fileName, int pin, const char* message) {
  dataFile = SD.open(fileName, FILE_WRITE);
  if (dataFile) {
    int sensorValue = analogRead(pin);
    float voltageMeasuredInMilliVolts = sensorValue * (5.0 / 1023.0) * 1000 / VOLTAGE_DIVIDER;
    float powerGeneratedInMicrowatts = voltageMeasuredInMilliVolts * voltageMeasuredInMilliVolts / RESISTANCE_IN_OHMS;
    dataFile.print(sensorValue, 3);
    dataFile.print(",");
    dataFile.print(voltageMeasuredInMilliVolts, 3);
    dataFile.print(",");
    dataFile.println(powerGeneratedInMicrowatts, 6);
    Serial.print(message);
    Serial.print(sensorValue, 3);
    Serial.print(",");
    Serial.print(voltageMeasuredInMilliVolts, 3);
    Serial.print(",");
    Serial.println(powerGeneratedInMicrowatts, 6);
    dataFile.close();
  } else {
    Serial.print("error opening ");
    Serial.println(fileName);
  }
  //delay(250);
}
