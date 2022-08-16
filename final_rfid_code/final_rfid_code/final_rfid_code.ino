#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include "RTClib.h"
#include <SD.h>

#define RST_PIN         9     //RST pins of two rc522s parallel
#define SS_1_PIN        4     //for exit      //ss pin of rc522 for exit
#define SS_2_PIN        8     //for entrance  //ss pin of rc522 for entrance
#define NR_OF_READERS   2     //number of rc522 modules
#define SS_Of_SD        10    //ss pin of SD module
#define Lockpin         3     //lock pin 
#define Adress_LCD_I2C  0x27  //adress of LCD I2C

byte ssPins[] = {SS_1_PIN, SS_2_PIN};
String struid = "";
char daysOfTheWeek[7][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
unsigned long timeopen = 0;
unsigned long timenow = 0;
int timeofBeingOpen = 5;
int uidSize = 0;
byte uid[8];
String mastercardOne = "xxxxxxxx"; //uid of master card one
String mastercardTwo = "xxxxxxxx"; //uid of master card two

RTC_DS3231 rtc;
MFRC522 mfrc522[NR_OF_READERS];
LiquidCrystal_I2C lcd(Adress_LCD_I2C , 16, 2);
File writingfile;
File readingfile;


void setup() {

  Serial.begin(9600);           // Initialize serial communications with the PC
  SPI.begin();                  // Init SPI bus
  lcd.backlight();

  while (! rtc.begin()) {       //DS3231 is intialized
    lcd.setCursor(0, 0);
    lcd.print("Init'ing Failed:");
    lcd.setCursor(0, 1);
    lcd.print("DS3231 module");
    delay(1000);
  }

  while (!SD.begin(SS_Of_SD)) { //Is SD card in access
    lcd.setCursor(0, 0);
    lcd.print("Init'ing Failed:");
    lcd.setCursor(0, 1);
    lcd.print("microSD module");
    delay(1000);
  }

  for (int reader = 0; reader < NR_OF_READERS; reader++) {    //Init all RC522 modules
    mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN);
    mfrc522[reader].PCD_DumpVersionToSerial();                //***TEST IF WE CAN DELETE THIS LINE OR NOT***
  }

  pinMode(Lockpin, OUTPUT);
  digitalWrite (Lockpin , LOW);

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));             //update time with system

  lcd.setCursor(3, 0);
  lcd.print("Nouvo Team");
  lcd.setCursor(4, 1);
  lcd.print("Presents");
  delay(3000);
}



void loop() {

  for (int reader = 0; reader < NR_OF_READERS; reader++) {                               //be aware if reader is 0 it means "SS_1_PIN" and in code its for EXIT
    Doorlock ("check");
    if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial()) {           // Looking for new cards from all RC522s
      dump_byte_array(mfrc522[reader].uid.uidByte, mfrc522[reader].uid.size);                         //***TEST IF WE CAN DELETE THIS LINE OR NOT***
      uidSize = 0;
      uidSize = mfrc522[reader].uid.size;
      memcpy(uid, mfrc522[reader].uid.uidByte, uidSize);
      struid = "";
      for (byte i = 0; i < uidSize; i++)                      //making UID bytes to the string
        struid += String(uid[i], HEX);
      if (reader == 0) {                                      //exit rc522
        if (struid == mastercardOne || struid == mastercardTwo) {             //check its one of Master Cards or not
          do {                                                                //waiting for new card after master card
            uidSize = 0;
            struid = "";
            if (mfrc522[0].PICC_IsNewCardPresent() && mfrc522[0].PICC_ReadCardSerial()) {
              dump_byte_array(mfrc522[0].uid.uidByte, mfrc522[0].uid.size);       //***TEST IF WE CAN DELETE THIS LINE OR NOT***
              uidSize = mfrc522[0].uid.size;
              memcpy(uid, mfrc522[0].uid.uidByte, uidSize);
              for (byte i = 0; i < uidSize; i++)
                struid += String(uid[i], HEX);
            }
          } while (uidSize == 0);

          if (SD.exists(struid)) {          //check if new card exists then show its UID otherwise make a new txt file and enter for that
            Doorlock ("check");
            lcd.setCursor(2, 0);
            lcd.print("UID: " + struid);
            lcd.setCursor(0, 1);
            lcd.print("exists now");
            delay(3000);
          }
          else {
            Doorlock ("check");
            SDwrite ("enter", struid);
            lcd.setCursor(1, 0);
            lcd.print("new card added");
            lcd.setCursor(0, 1);
            lcd.print("UID: " + struid);
            delay(3000);
          }
        }

        else if (SD.exists(struid)) {   //if its not master card then check the card exists or not (if it exists then write and show EXIT and open the Door)
          SDwrite ("exit", struid);
          Doorlock ("open");
          lcd.setCursor(6, 0);
          lcd.print("exit");
          lcd.setCursor(4, 1);
          lcd.print(TimeOrDate("Time"));
          delay(1000);
        }
        else {                         //if it doesnt exists show not allow or wrong card
          Doorlock ("check");
          lcd.setCursor(2, 0);
          lcd.print("not allow or");
          lcd.setCursor(2, 1);
          lcd.print("wrong card!");
          delay(1000);
        }
      }
      else {                              //if reader *isnt* 0 then it must be one so its from entrance rc522
        if (SD.exists(struid)) {          //check if card exists or not (if it exists then write and show ENTER and open the door)
          SDwrite ("enter", struid);
          Doorlock ("open");
          lcd.setCursor(5, 0);
          lcd.print("enter");
          lcd.setCursor(4, 1);
          lcd.print(TimeOrDate("Time"));
          delay(1000);
        }
        else {                           //if it doesnt exists show not allow or wrong card
          Doorlock ("check");
          lcd.setCursor(2, 0);
          lcd.print("not allow or");
          lcd.setCursor(2, 1);
          lcd.print("wrong card!");
          delay(1000);
        }
      }
    }
    else {                          //if no card presents show wainting for a card on lcd With Date and Time
      lcd.setCursor(0, 0);
      lcd.print("Present your Card");
      lcd.setCursor(1, 1);
      lcd.print(TimeOrDate("Date") + " " + TimeOrDate("Time"));
      delay(100);
    }
  }
  Doorlock ("check");
}


void dump_byte_array(byte * buffer, byte bufferSize) {      //***TEST IF WE CAN DELETE THIS FUNCTION OR NOT***
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

void SDwrite (String possition , String uid) {    //SDwrite gets the name of txt file and state of EXIT or ENTER the write on the txt file
  DateTime nowTime = rtc.now();
  int x = nowTime.dayOfTheWeek();
  uid += ".txt";
  writingfile = SD.open(uid, FILE_WRITE);
  writingfile.print(String(daysOfTheWeek[x]) + ": " + nowTime.day() + "/" + nowTime.month() + "/" + nowTime.year() + " -> " + nowTime.hour() + ":" + nowTime.minute() + ":" + nowTime.second());
  if (possition == "exit")
    writingfile.println(" exit");
  else if (possition == "enter")
    writingfile.println(" enter");
  else
    Serial.println("...Error in possition SDwrite...");
  writingfile.close();
}

String SDread (String uid) {        //SDread gets the name of txt file and read the last state if it was ENTER or EXIT
  uid += ".txt";
  readingfile = SD.open(uid, FILE_READ);
  readingfile.seek(readingfile.size() - 1);       //-1 beacuse last char of exiT or enteR //maybe it must be -2 or even without minus!
  char endchar = readingfile.read();
  readingfile.close();
  if (endchar == "t")
    return "exit";
  else if (endchar == "r")
    return "enter";
  else
    Serial.println("...Error in SDread function...");
}

void Doorlock (String possition) {      //Doorlock gets two states, open and check
  if (possition == "open") {            //open : open the door
    digitalWrite(Lockpin , HIGH);
    timeopen = millis() / 1000;         //save the time when the door opened
  }
  else if (possition == "check") {      //check: close the door after "timeofBeingOpen" seconds
    if (digitalRead (Lockpin) == HIGH) {  //check if the door is open now or not
      timenow = millis() / 1000;
      if ((timenow - timeopen) > timeofBeingOpen)
        digitalWrite(Lockpin , LOW);
    }
  }
  else
    Serial.println("...Error in doorlock function...");
}

String TimeOrDate (String TorD) {     //TimeOrDate gets two states,Time for returning current time and Date for returning current date
  DateTime nowTime = rtc.now();
  if (TorD == "Time") {
    String Time = String( nowTime.hour() ) + ":" + nowTime.minute() + ":" + nowTime.second();
    return Time ;
  }
  else if (TorD == "Date") {
    String Date = nowTime.day() + "/" + nowTime.month();
    return Date ;
  }
  else
    Serial.println("...Error in tiam or date function...");
}
