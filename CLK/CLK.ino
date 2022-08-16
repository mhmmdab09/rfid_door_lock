#include <Wire.h>     //needed because DS3231 uses I2C Bus
#include <RTClib.h>   //needed becuase we have ready-made functions of this librray
RTC_DS3231 rtc;     //the object rtc is created from the class RTC_DS3231
char daysOfTheWeek[7][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

void setup()
{
  Serial.begin(9600);
  /*
  while (! rtc.begin())   //DS3231 is intialized
  {
    Serial.println("waiting ...");
    delay(300);
  }
  */
   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));//auto update from computer time
   //rtc.adjust(DateTime(2014, 1, 21, 3, 10, 17));//set date-time manualy:yr,mo,dy,hr,mn,se
}

void  loop()
{
    //--read Date and Tme from DS3231 using the method now()---------------------;
    //--store the Date and Time into another user define object bamed nowTime
    DateTime nowTime = rtc.now();
  
    //---show Day and Date on Top Line of LCD--------------
    int x = nowTime.dayOfTheWeek(); //dyOfTheWeek() is a pre-defined method
    Serial.print(daysOfTheWeek[x]);    //days of the Week[] is the user defined array
    Serial.print(": ");
    
    Serial.print(nowTime.day());
    Serial.print("/");

    Serial.print(nowTime.month());
    Serial.print("/");

    Serial.print(nowTime.year());

    //------show Time on Bottom Line of LCD--------------
    Serial.print(" -> ");
    Serial.print(nowTime.hour());
    Serial.print(":");

    Serial.print(nowTime.minute());
    Serial.print(":");

    Serial.print(nowTime.second());
    Serial.println();
    delay(1000);
  
}
