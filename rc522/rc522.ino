
#include <SPI.h>
#include <MFRC522.h>
#include "RTClib.h"
#include <SD.h>

#define RST_PIN         9
#define SS_1_PIN        4
#define SS_2_PIN        8
#define NR_OF_READERS   2

byte ssPins[] = {SS_1_PIN, SS_2_PIN};
String struid = "";

MFRC522 mfrc522[NR_OF_READERS];


  
void setup() {

  Serial.begin(9600);           // Initialize serial communications with the PC

  SPI.begin();                  // Init SPI bus


  /* looking for MFRC522 readers */
  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN);
    mfrc522[reader].PCD_DumpVersionToSerial();
  }
}



void loop() {

  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {

    // Looking for new cards
    if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial()) {
      // Show some details of the PICC (that is: the tag/card)
      dump_byte_array(mfrc522[reader].uid.uidByte, mfrc522[reader].uid.size);

      int uidSize = mfrc522[reader].uid.size;
      byte uid[8];

      memcpy(uid, mfrc522[reader].uid.uidByte, uidSize);
      for (byte i = 0; i < uidSize; i++) {
        struid += String(uid[i], HEX);
      }
    
    
    }
    else{
      //lcd.print("waiting for a card...")
      //lcd.print("date & time") 
    }
  } 
  Serial.println(struid);
}


void dump_byte_array(byte * buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
