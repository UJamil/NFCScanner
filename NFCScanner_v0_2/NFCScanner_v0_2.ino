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
MFRC522::MIFARE_Key key;

void setup() {
  Serial.begin(9600);	// Initialize serial communications with the PC
  SPI.begin();			// Init SPI bus
  mfrc522.PCD_Init();	// Init MFRC522 card

  // Prepare the key (used both as key A and as key B)
  // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println("Scan PICC to see UID and type...");
}

void loop() {

  // In this sample we use the second sector,
  // that is: sector #1, covering block #4 up to and including block #7
  byte sector         = 1;
  byte block          = 4;
  byte dataBlock[]    = {
    0x01, 0x02, 0x03, 0x04, //  1,  2,   3,  4,
    0x05, 0x06, 0x07, 0x08, //  5,  6,   7,  8,
    0x08, 0x09, 0xff, 0x0b, //  9, 10, 255, 12,
    0x0c, 0x0d, 0x0e, 0x0f  // 13, 14,  15, 16
  };
  byte trailerBlock   = 7;
  byte status;
  byte buffer[18];
  byte len;
  byte size = sizeof(buffer);

  if (!mfrc522.PICC_IsNewCardPresent()){

  }

  // Authenticate using key A
  //  Serial.println("Authenticating using key A...");
  //  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  //  if (status != MFRC522::STATUS_OK) {
  //    Serial.print("PCD_Authenticate() failed: ");
  //    Serial.println(mfrc522.GetStatusCodeName(status));
  //    return;
  //  }
  //
  //  // Show the whole sector as it currently is
  //  Serial.println("Current data in sector:");
  //  mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
  //  Serial.println();
  //
  //  // Read data from the block
  //  Serial.print("Reading data from block "); 
  //  Serial.print(block);
  //  Serial.println(" ...");
  //  status = mfrc522.MIFARE_Read(block, buffer, &size);
  //  if (status != MFRC522::STATUS_OK) {
  //    Serial.print("MIFARE_Read() failed: ");
  //    Serial.println(mfrc522.GetStatusCodeName(status));
  //  }
  //  Serial.print("Data in block "); 
  //  Serial.print(block); 
  //  Serial.println(":");
  //  dump_byte_array(buffer, 16); 
  //  Serial.println();
  //  Serial.println();
  //
  //  // Authenticate using key B
  //  Serial.println("Authenticating again using key B...");
  //  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522.uid));
  //  if (status != MFRC522::STATUS_OK) {
  //    Serial.print("PCD_Authenticate() failed: ");
  //    Serial.println(mfrc522.GetStatusCodeName(status));
  //    return;
  //  }
  //
  //
  //  // Dump the sector data
  //  Serial.println("Current data in sector:");
  //  mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
  //  Serial.println();
  //
  //  // Halt PICC
  //  mfrc522.PICC_HaltA();
  //  // Stop encryption on PCD
  //  mfrc522.PCD_StopCrypto1();
  //

  //WRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITE
  // Look for new cards, write
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

  //WRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITEWRITE

  //  // Select one of the cards
  //  if ( ! mfrc522.PICC_ReadCardSerial()) {
  //    return;
  //  }
  //  // Dump debug info about the card. PICC_HaltA() is automatically called.
  //  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
  Serial.println(" ");
  mfrc522.PICC_HaltA(); // Halt PICC
  mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
}


/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}



