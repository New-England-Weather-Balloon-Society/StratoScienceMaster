#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#include <SPI.h>
#include <SD.h>

#define CUTDOWN_HELO 9
#define CUTDOWN_GLIDER 10
#define CUTDOWN_HELO_TOAPRS 7
#define CUTDOWN_GLIDER_TOAPRS 8
#define BMP_SCK 13
#define BMP_MISO 12
#define BMP_MOSI 11
#define BMP_CS 10
float SEALEVELPRESSURE_HPA (1013.25);

Adafruit_BMP3XX bmp;

//#include <I2C.h>
int uv_ain = A0;
int ad_value;
int uv_ain1 = A1;
int ad_value1;
int bmpalt;

int wm_pin = A2;

const float voltageDivider = .25;
const int resistanceInOhms = 120; // 120 Ohms

const int chipSelect = 4;

const String WM_DATA_FILE_NAME = "windmill.txt";


void setup() {
  // Open serial communications and wait for port to open:
  pinMode(uv_ain, INPUT);
  pinMode(uv_ain1, INPUT);
  pinMode(CUTDOWN_HELO, OUTPUT);
  pinMode(CUTDOWN_GLIDER, OUTPUT);
  pinMode(CUTDOWN_HELO_TOAPRS, OUTPUT);
  pinMode(CUTDOWN_GLIDER_TOAPRS, OUTPUT);
  
  ////Set cutdown pins to LOW
  digitalWrite(CUTDOWN_HELO, LOW);
  digitalWrite(CUTDOWN_HELO_TOAPRS, LOW);
  digitalWrite(CUTDOWN_GLIDER, LOW);
  digitalWrite(CUTDOWN_GLIDER_TOAPRS, LOW);

  Serial.begin(9600);

    while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }
  //Serial.begin(115200);
  while (!Serial)
    ;
  //Serial.println("Adafruit BMP388 / BMP390 test");


  if (!bmp.begin_I2C()) {  // hardware I2C mode, can pass in address & alt Wire
                           //if (! bmp.begin_SPI(BMP_CS)) {  // hardware SPI mode
                           //if (! bmp.begin_SPI(BMP_CS, BMP_SCK, BMP_MISO, BMP_MOSI)) {  // software SPI mode
    Serial.println("Could not find a valid BMP3 sensor, check wiring!");
    while (1)
      ;
  }


  // Set up oversampling and filter initialization
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);

  bmp.readAltitude(SEALEVELPRESSURE_HPA);

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1)
      ;
  }
  Serial.println("card initialized.");
  //SEALEVELPRESSURE_HPA = bmp.pressure / 100.0;
}

void loop() {

  /////BMP380 code
  /*if (!bmp.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }
  Serial.print("Temperature = ");
  Serial.print(bmp.temperature);
  Serial.println(" *C");

  Serial.print("Pressure = ");
  Serial.print(bmp.pressure / 100.0);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(bmp.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.println();*/

  ////CUTDOWN CODE
  bmpalt = (bmp.readAltitude(SEALEVELPRESSURE_HPA));

  ///If altitude is above 80k ft
  if (bmpalt >= 24384){
    delay(500);
    ///Check again
    bmpalt = (bmp.readAltitude(SEALEVELPRESSURE_HPA));
      if (bmpalt >= 24384){
          ///CUT DOWN THE GLIDER
          digitalWrite(CUTDOWN_GLIDER, HIGH);
          delay (5000);
          digitalWrite(CUTDOWN_GLIDER, LOW);
          digitalWrite(CUTDOWN_GLIDER_TOAPRS, HIGH);
      }
  }

  ///If altitude is above 81k ft
  if (bmpalt >= 24688){
    delay(500);
    ///Check again
    bmpalt = (bmp.readAltitude(SEALEVELPRESSURE_HPA));
      if (bmpalt >= 24688){
          ///CUT DOWN THE HELO
          digitalWrite(CUTDOWN_HELO, HIGH);
          delay (5000);
          digitalWrite(CUTDOWN_HELO, LOW);
          digitalWrite(CUTDOWN_HELO_TOAPRS, HIGH);
      }
  }


  /////UV Sensor code
  bmpalt = (bmp.readAltitude(SEALEVELPRESSURE_HPA));
  ad_value = analogRead(uv_ain);
  ad_value1 = analogRead(uv_ain1);  
  
  // make a string for assembling the data to log:
  String dataString0 = (String)ad_value;
  String dataString1 = (String)ad_value1;
  String dataString3 = (String)bmpalt;

  //dataString = dataString + "Temp: " + (String)DHT.temperature + " Humidity: " + (String)DHT.humidity;

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile0 = SD.open("datalog0.txt", FILE_WRITE);


  // if the file is available, write to it:
  if (dataFile0) {
    dataFile0.println(dataString0);

    // print to the serial port too:
    Serial.print("UV0: ");
    Serial.println(dataString0);
    dataFile0.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
  
  File dataFile1 = SD.open("datalog1.txt", FILE_WRITE);

  if (dataFile1) {
    dataFile1.println(dataString1);

    // print to the serial port too:
    Serial.print("UV1: ");
    Serial.println(dataString1);
    dataFile1.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog1.txt");
  }
  


///Write Pressure Sensor to SD Card

  File dataFile3 = SD.open("datalog3.txt", FILE_WRITE);

  if (dataFile3) {
    dataFile3.println(dataString3);
    dataFile3.close();

    // print to the serial port too:
    Serial.print("BMP388: "); 
    Serial.println(dataString3);
    //Serial.print("Pressure: ");
    Serial.println(bmp.pressure / 100.0);
    //Serial.print("Altitude: ");
    Serial.println(bmp.readAltitude(SEALEVELPRESSURE_HPA));
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog3.txt");
  }
  
  
  int sensorValue = analogRead(wm_pin);
  
  float voltageMeasuredInMilliVolts = sensorValue * (5.0 / 1023.0) * 1000 / voltageDivider ;
  float powerGeneratedInMicrowatts = voltageMeasuredInMilliVolts * voltageMeasuredInMilliVolts / resistanceInOhms;

  String buf;
  buf += String("WM:");
  buf += String(sensorValue, 3);
  buf += F(",");
  buf += String(voltageMeasuredInMilliVolts, 3);
  buf += F(",");
  buf += String(powerGeneratedInMicrowatts, 6);
  File wm_data_file = SD.open(WM_DATA_FILE_NAME, FILE_WRITE);

  // if the file is available, write to it:
  if (wm_data_file) {
    wm_data_file.println(buf);
    
    // print to the serial port too:
    Serial.println(buf);
    wm_data_file.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening " + WM_DATA_FILE_NAME);
  }
  



  delay(100);
}

