#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>

constexpr uint8_t RST_PIN = 49;
constexpr uint8_t SS_PIN = 53;

LiquidCrystal_I2C lcd(0x27, 16, 2); //Parameters: (rs, enable, d4, d5, d6, d7)
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

int buzzerPin = 8;
int relayPin = 9;

void setup() {
  pinMode(buzzerPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);

  lcd.begin();   // LCD screen
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522

  lcd.setCursor(0, 0);
  lcd.print(" RFID Door Lock");
  lcd.setCursor(0, 1);
  lcd.print("  by MakerPro");
  delay(3000);

  lcd.clear();
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print(" RFID Door Lock");
  lcd.setCursor(0, 1);
  lcd.print(" Show Your Tag ");
  
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  
  //Reading from the card
  String tag = "";
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    tag.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    tag.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  tag.toUpperCase();
  
  //Checking the card
  if (tag.substring(1) == "29 B9 ED 23") //change here the UID of the card/cards that you want to give access
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Granted");
    lcd.setCursor(0, 1);
    lcd.print("Door Opened");
    digitalWrite(relayPin, HIGH);
    delay(3000);
    digitalWrite(relayPin, LOW);
    lcd.clear();
  }

  else
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Wrong Tag Shown");
    lcd.setCursor(0, 1);
    lcd.print("Access Denied");
    digitalWrite(buzzerPin, HIGH);
    delay(3000);
    digitalWrite(buzzerPin, LOW);
    lcd.clear();
  }
}
