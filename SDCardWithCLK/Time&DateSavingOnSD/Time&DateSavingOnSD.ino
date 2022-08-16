#include <Wire.h>
#include <RTClib.h>
#include <SPI.h>
#include <SD.h>

File myFile;
RTC_DS3231 rtc;     //the object rtc is created from the class RTC_DS3231

char daysOfTheWeek[7][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

void setup()
{
  
  Serial.begin(9600);  //start the serial
  if (! rtc.begin()){   //DS3231 is intialized
    while (1);
  }
  
  Serial.println("Initializing SD card...");  //is sd card was in access or not
  if (!SD.begin(10)){
    Serial.println("initialization failed!");
    //while (1);
  }
  Serial.println("initialization done.");
  
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //update time witj system

  myFile = SD.open("time.txt", FILE_WRITE);
}

void  loop(){
    //--read Date and Tme from DS3231 using the method now()---------------------;
    //--store the Date and Time into another user define object bamed nowTime
    DateTime nowTime = rtc.now();
    myFile = SD.open("time.txt", FILE_WRITE);
    //---show Day and Date on Top Line of LCD--------------
    int x = nowTime.dayOfTheWeek(); //dyOfTheWeek() is a pre-defined method
    if(!myFile){
      Serial.println("error opening file");
    }
    else{
      myFile.print(daysOfTheWeek[x]);    //days of the Week[] is the user defined array
      myFile.print(": ");
      
      myFile.print(nowTime.day());
      myFile.print("/");
  
      myFile.print(nowTime.month());
      myFile.print("/");
  
      myFile.print(nowTime.year());
  
      //------show Time on Bottom Line of LCD--------------
      myFile.print(" -> ");
      myFile.print(nowTime.hour());
      myFile.print(":");
  
      myFile.print(nowTime.minute());
      myFile.print(":");
  
      myFile.print(nowTime.second());
      myFile.println();

      //-------code done once-----------
      Serial.println("done once");
    }
    delay(1000);
    myFile.close();
  
}
