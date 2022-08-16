/*
#####################################################################################
#  	Arquivo:            RFID_Simples.ino                                             
#       Micro-processador:  Arduino UNO ou Teensy 2.0++      
#  	Linguagem:	    Wiring / C /Processing /Fritzing / Arduino IDE
#       Versão:             1.0
#						
#	Objetivo:           Arduino RFID - Sistema de Segurança e Controle de Acessos
#										  
#	Funcionamento:	    Utilizando o RFID MFCR-522AN, controlar o acesso de pessoas
#                           autorizadas em um ambiente.
#                           Para isso verificar o cartão ou token de RF; sendo válido:
#                           acender um LED, tocar uma música e acionar um servo-motor
#                           de abertura de porta - sendo inválido, rejeitar o acesso
#                           piscando um LED, tocando uma música e mantendo a porta 
#                           travada.
#                           Um LCD de 20 caracteres x 4 linhas mostra as mensagens 
#                           referentes à liberação ou negação do acesso.
#
#			
#   Autor:              Marcelo Moraes 
#   Data:               27/05/13	
#   Local:              Sorocaba - SP	
#					
#####################################################################################
 
Este projeto contém código de domínio público.
A modificação é permitida sem aviso prévio.
*/

// inclusão das bilibotecas
#include <SPI.h>
#include <RFID.h>
#include <Servo.h>
#include "pitches.h"
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// definição do RFID
RFID rfid(10,5);

byte emaomos[5] = {0xDE,0x1F,0x47,0xC9,0x4F}; // Marcelo Moraes (e+mao+mos)
// coloque aqui seus outros cartões permitidos

// declação do LCD, endereço e tipo
LiquidCrystal_I2C lcd(0x27,16,2);// declara LCD, endereço e tipo

byte serNum[5];
byte data[5];

// definição das melodias de acesso/boas vindas e rejeição/erro
int access_melody[] = {NOTE_G4,0,NOTE_A4,0, NOTE_B4,0,NOTE_A4,0,NOTE_B4,0, NOTE_C5,0};
int access_noteDurations[] = {8,8,8,8,8,4,8,8,8,8,8,4};
int fail_melody[] = {NOTE_G2,0,NOTE_F2,0,NOTE_D2,0};
int fail_noteDurations[] = {8,8,8,8,8,4};

// definição dos pinos do LED, Buzzer e Servo-motor
int LED_access = 2;
int LED_intruder = 3;
int speaker_pin = 8;
int servoPin = 9;

// definição do servo-motor
Servo doorLock;


void setup(){
  doorLock.attach(servoPin); // vinculando servo-motor ao pino
  Serial.begin(9600); // inicialização da comunicação serial
  lcd.init(); // inicializa o LCD
  lcd.backlight();
  lcd.clear();// limpa LCD
  SPI.begin(); // inicialização da comunicação SPI para o RFID
  rfid.init(); // inicialização do RFID
  Serial.println("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
  Serial.println("+ http://arduinobymyself.blogspot.com.br                                        +"); 
  Serial.println("+ Arduino Sistema de Segurança com o RFID: MFRC522-AN                           +");
  Serial.println("+ Módulo RFID iniciado no modo de leitura automática, Esperando por um cartão...=");
  Serial.println("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
  delay(1000);
  pinMode(LED_access,OUTPUT);
  pinMode(LED_intruder,OUTPUT);
  pinMode(speaker_pin,OUTPUT);
  pinMode(servoPin,OUTPUT);
}
 
void loop(){
  lcd.clear();
  lcd.noBacklight();
  // aqui voce vai criar uma variável para cada usuário
  // INICIALDONOME_card ou CODIGO_card
  boolean emaomos_card = true; // meu cartão
  
  if (rfid.isCard()){ // encontrado cartão válido
    if (rfid.readCardSerial()){ // faz a leitura do cartão
      delay(1000);
      data[0] = rfid.serNum[0]; // armazena o número serial
      data[1] = rfid.serNum[1];
      data[2] = rfid.serNum[2];
      data[3] = rfid.serNum[3];
      data[4] = rfid.serNum[4];
    }
  
  //rfid.halt(); // RFID para standby
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("   ID encontrado:");
  //Serial.print("Cartao encontrado - code:");
  lcd.setCursor(0,1);
  lcd.print("     ");
  if(data[0] < 16){
    lcd.print("0");
  }
  lcd.print(data[0],HEX);
  
  if(data[1] < 16){
    lcd.print("0");
  }
  lcd.print(data[1],HEX);
  
  if(data[2] < 16){
    lcd.print("0");
  }
  lcd.print(data[2],HEX);
  
  if(data[3] < 16){
    lcd.print("0");
  }
  lcd.print(data[3],HEX);
  
  if(data[4] < 16){
    lcd.print("0");
  }
  lcd.print(data[4],HEX);
  for(int i=0; i<5; i++){
    if(data[i] != emaomos[i]) emaomos_card = false; // se não for meu cartão, coloca como falso
    // aqui você pode verificar os outros cartões permitidos
  }
  Serial.println();
  if (emaomos_card){ // se um cartão foi encontrado
    lcd.setCursor(0,2);
    lcd.print("    Ola Marcelo!");
    //Serial.println("Ola Marcelo!"); // imprime mensagem
    for (int i = 0; i < 12; i++){ // toca música de boas vindas
      int access_noteDuration = 1000/access_noteDurations[i];
      tone(speaker_pin, access_melody[i],access_noteDuration);
      int access_pauseBetweenNotes = access_noteDuration * 1.30;
      delay(access_pauseBetweenNotes);
      noTone(speaker_pin);
    }
  }
  /*
  //analise dos outros cartões
  else if(USER_card// ponha seus outros usuarios aqui){
    lcd.setCursor(0,3);
    lcd.print("Ola NOME_DE_USUARIO");
    //Serial.println("Ola NOME_DE_USUARIO");
    for (int i = 0; i < 12; i++){
      int access_noteDuration = 1000/access_noteDurations[i];
      tone(speaker_pin, access_melody[i],access_noteDuration);
      int access_pauseBetweenNotes = access_noteDuration * 1.30;
      delay(access_pauseBetweenNotes);
      noTone(speaker_pin);
    }
  }
  */
  else{ // se um cartão desconhecido foi emcontrado
    lcd.setCursor(0,2);
    lcd.print(" ID nao reconhecido!");
    lcd.setCursor(0,1);
    lcd.print("   Acesso Negado!");
    //Serial.println("Cartao nao reconhecido!  Contacte o administrador!"); // imprime mensagem
    digitalWrite(LED_intruder, HIGH); // acende LED vermelho
    for (int i = 0; i < 6; i++){ // toca música de rejeição de usuário
      int fail_noteDuration = 1000/fail_noteDurations[i];
      tone(speaker_pin, fail_melody[i],fail_noteDuration);
      int fail_pauseBetweenNotes = fail_noteDuration * 1.30;
      delay(fail_pauseBetweenNotes);
      noTone(speaker_pin);
    }
    delay(1000);
    digitalWrite(LED_intruder, LOW); // apaga LED vermelho
  }
  if (emaomos_card){// adicione os outros usuários com um ou logico ||
    // mensagem de boas vindas e permissão de acesso  
    lcd.setCursor(0,2);
    lcd.print("  Acesso Garantido!");
    lcd.setCursor(0,1);
    lcd.print("   Seja bem Vindo!");
    //Serial.println("Acesso garantido!... Seja bem vindo as dependencias do ArduinoByMySelf!");
    digitalWrite(LED_access,HIGH); // acende LED verde
    doorLock.write(180); // libera porta
    delay(5000); // espera
    doorLock.write(0); // trava porta
    digitalWrite(LED_access,LOW); // apaga LED verde
  }
  Serial.println();
  delay(500);
  rfid.halt();
  lcd.noBacklight();
  }
}
  
