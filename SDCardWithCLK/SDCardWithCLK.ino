#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "Sodaq_DS3231.h"

File myFile;
DateTime now;

int newMinute = 0;
int oldMinute = 0;

void save_temperature() {
  myFile = SD.open("temp.txt", FILE_WRITE);
  now = rtc.now();
  myFile.print(now.hour());
  myFile.print(":");
  myFile.print(now.minute());
  rtc.convertTemperature(); //convert current temperature into registers
  myFile.print(",");
  myFile.println(rtc.getTemperature()); //read registers and save temperature on deg C
  myFile.close();
}

void setup (){
  Wire.begin();
  rtc.begin();
  Serial.begin(9600);
  Serial.print("Initializing SD card...");
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
  if(myFile.available()){
    Serial.println("file found");
  }
  else{
    Serial.println("file not found");
  }
  now = rtc.now();
  oldMinute = now.hour();
}

void loop (){
  now = rtc.now();
  newMinute = now.minute();
  if (oldMinute != newMinute) {
    save_temperature();
    oldMinute = newMinute;
  }
}
