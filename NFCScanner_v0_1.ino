/*
 * MFRC522 - Library to use ARDUINO RFID MODULE KIT 13.56 MHZ WITH TAGS SPI W AND R BY COOQROBOT.
 * The library file MFRC522.h has a wealth of useful info. Please read it.
 * The functions are documented in MFRC522.cpp.
 *
 * Based on code Dr.Leong   ( http://WWW.B2CQSHOP.COM )
 * Created by Miguel Balboa (circuitito.com), Jan, 2012.
 * Rewritten by Søren Thing Andersen ( http://access.thing.dk ), fall of 2013 (Translation to English, refactored, comments, anti collision, cascade levels.)
 * Released into the public domain.
 *
 * Sample program showing how to read data from a PICC using a MFRC522 reader on the Arduino SPI interface.
 *----------------------------------------------------------------------------- empty_skull 
 * Aggiunti pin per arduino Mega
 * add pin configuration for arduino mega
 * http://mac86project.altervista.org/
 ----------------------------------------------------------------------------- Nicola Coppola
 * Pin layout should be as follows:
 * Signal     Pin              Pin               Pin
 *            Arduino Uno      Arduino Mega      MFRC522 board
 * ------------------------------------------------------------
 * Reset      9                5                 RST
 * SPI SS     10               53                SDA
 * SPI MOSI   11               51                MOSI
 * SPI MISO   12               50                MISO
 * SPI SCK    13               52                SCK
 *
 */

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);	// Create MFRC522 instance.

void setup() {
  Serial.begin(9600);	// Initialize serial communications with the PC
  SPI.begin();			// Init SPI bus
  mfrc522.PCD_Init();	// Init MFRC522 card
  Serial.println("Scan PICC to see UID and type...");
}

void loop() {

  byte buffer[34];  
  byte block;
  byte status, len;

  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  // Look for new cards
  if ( mfrc522.PICC_IsNewCardPresent() == true) {
    if ( ! mfrc522.PICC_ReadCardSerial()) {
      return;
    }
    Serial.write(1);
    Serial.println("1");
    Serial.setTimeout(20000L) ;     // wait until 20 seconds for input from serial

    // Ask for input
    Serial.println("Input Object Name name + #");
    len=Serial.readBytesUntil('#', (char *) buffer, 30) ; // read family name from serial
    for (byte i = len; i < 30; i++) buffer[i] = '\s';     // pad with spaces

    block = 4;
    //Serial.println("Authenticating using key A...");
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
      Serial.print("PCD_Authenticate() failed: ");
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }

    // Write block
    status = mfrc522.MIFARE_Write(block, buffer, 16);
    if (status != MFRC522::STATUS_OK) {
      Serial.print("MIFARE_Write() failed: ");
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }
    else Serial.println("MIFARE_Write() success: ");

    block = 5;
    //Serial.println("Authenticating using key A...");
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
      Serial.print("PCD_Authenticate() failed: ");
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }

    // Write block
    status = mfrc522.MIFARE_Write(block, &buffer[16], 16);
    if (status != MFRC522::STATUS_OK) {
      Serial.print("MIFARE_Write() failed: ");
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }
    else Serial.println("MIFARE_Write() success: ");
    Serial.println(" ");
    mfrc522.PICC_HaltA(); // Halt PICC
    mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
    return;
  }

  //  // Dump debug info about the card. PICC_HaltA() is automatically called.
  //  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
}







