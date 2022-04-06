// Date and time functions using RX8025 RTC connected via I2C and Wire lib

#include <Wire.h>
#include <DS3231.h>
#include <RTClib.h>
RTC_DS3231 rtc;

char weekDay[][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

void setup () 
{
    Serial.begin(9600);
    Wire.begin();
    rtc.begin();
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

uint32_t old_ts;

void loop () 
{
    DateTime now = rtc.now(); //get the current date-time
    uint32_t ts = now.getEpoch();

	Serial.print(now.year(), DEC);
	Serial.print('/');
	Serial.print(now.month(), DEC);
	Serial.print('/');
	Serial.print(now.date(), DEC);
	Serial.print(' ');
	Serial.print(now.hour(), DEC);
	Serial.print(':');
	Serial.print(now.minute(), DEC);
	Serial.print(':');
	Serial.print(now.second(), DEC);
	Serial.print(' ');
	Serial.print(weekDay[now.dayOfWeek()]);
	Serial.println();
	Serial.print("Seconds since Unix Epoch: "); 
	Serial.print(ts, DEC);
	Serial.println();
  delay(1000);
}
