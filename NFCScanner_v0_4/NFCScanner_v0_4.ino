/*
 _   _ ______ _____   _____                                 
 | \ | ||  ___/  __ \ /  ___|                                
 |  \| || |_  | /  \/ \ `--.  ___ __ _ _ __  _ __   ___ _ __ 
 | . ` ||  _| | |      `--. \/ __/ _` | '_ \| '_ \ / _ \ '__|
 | |\  || |   | \__/\ /\__/ / (_| (_| | | | | | | |  __/ |   
 \_| \_/\_|    \____/ \____/ \___\__,_|_| |_|_| |_|\___|_|   
 
 */
/* Turn an LED on/off based on a command send via BlueTooth
 **
 ** Credit: The following example was used as a reference
 ** Rui Santos: http://randomnerdtutorials.wordpress.com
 */
/*
 * Write personal data of a MIFARE RFID card using a RFID-RC522 reader
 * Uses MFRC522 - Library to use ARDUINO RFID MODULE KIT 13.56 MHZ WITH TAGS SPI W AND R BY COOQROBOT. 
 ----------------------------------------------------------------------------- 
 * Pin layout should be as follows:
 * Signal     Pin              Pin               Pin
 *            Arduino Uno      Arduino Mega      MFRC522 board
 * ------------------------------------------------------------
 * Reset      9                5                 RST
 * SPI SS     10               53                SDA
 * SPI MOSI   11               52                MOSI
 * SPI MISO   12               51                MISO
 * SPI SCK    13               50                SCK
 *
 * Hardware required:
 * Arduino
 * PCD (Proximity Coupling Device): NXP MFRC522 Contactless Reader IC
 * PICC (Proximity Integrated Circuit Card): A card or tag using the ISO 14443A interface, eg Mifare or NTAG203.
 * The reader can be found on eBay for around 5 dollars. Search for "mf-rc522" on ebay.com. 
 */
/**
 * ----------------------------------------------------------------------------
 * This is a MFRC522 library example; see https://github.com/miguelbalboa/rfid
 * for further details and other examples.
 * 
 * NOTE: The library file MFRC522.h has a lot of useful info. Please read it.
 * 
 * Released into the public domain.
 * ----------------------------------------------------------------------------
 * This sample shows how to read and write data blocks on a MIFARE Classic PICC
 * (= card/tag).
 * 
 * BEWARE: Data will be written to the PICC, in sector #1 (blocks #4 to #7).
 * 
 * 
 * Typical pin layout used:
 * ------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino
 *             Reader/PCD   Uno           Mega      Nano v3
 * Signal      Pin          Pin           Pin       Pin
 * ------------------------------------------------------------
 * RST/Reset   RST          9             5         D9
 * SPI SS      SDA(SS)      10            53        D10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11
 * SPI MISO    MISO         12 / ICSP-1   50        D12
 * SPI SCK     SCK          13 / ICSP-3   52        D13
 */
//------------------------------------------------------------------------------------------------------


#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);	// Create MFRC522 instance.
MFRC522::MIFARE_Key key;

int state = 0;
int flag = 0;
int ledPin = 7;  // use the built in LED on pin 13 of the Uno

void setup() {
  Serial.begin(9600);	// Initialize serial communications with the PC
  SPI.begin();			// Init SPI bus
  mfrc522.PCD_Init();	// Init MFRC522 card
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Prepare the key (used both as key A and as key B)
  // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println("Scan PICC to see UID and type...");
}

void loop(){

  //if some data is sent, read it and save it in the state variable
  if(Serial.available() > 0){
    state = Serial.read();
  }

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
    return;
  //true if ...IsNewCardPresent() == FALSE

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
    return;
  //true if ...ReadCardSerial() == FALSE

  // Sector #1, covering block #4 up to and including block #7 (other blocks are reserved for communication)
  byte sector         = 1;
  byte blockAddr      = 4;
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

  // Authenticate using key A
  Serial.println("Authenticating using key A...");
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("PCD_Authenticate() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }



  if (state == '1') {
    //    writeNFC (blockAddr, buffer, len, status, state);
    digitalWrite(ledPin, HIGH);
    Serial.println("WRITE MODE ENABLED");
  }
  else if(state == '0'){  
    readNFC (blockAddr, buffer, size, status, sector);
    digitalWrite(ledPin, LOW);
    Serial.println("WRITE MODE DISABLED");
  }

  // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();

  delay(500);
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



/**
 * Read the NFC Card
 */
void readNFC(byte blockAddr, byte *buffer, byte size, byte status, byte sector){
  // Read data from the block
  Serial.print("Reading data from block "); 
  Serial.print(blockAddr);
  Serial.println(" ...");
  status = mfrc522.MIFARE_Read(blockAddr, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("MIFARE_Read() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  Serial.print("Data in block "); 
  Serial.print(blockAddr); 
  Serial.println(":");
  dump_byte_array(buffer, 16); 
  Serial.println();
  Serial.println();

  // Show the whole sector as it currently is
  Serial.println("Current data in sector:");
  mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
  Serial.println();
  Serial.write((char *) buffer);
}

/**
 * Write to the NFC Card
 */
void writeNFC(byte blockAddr, byte *buffer, byte len, byte status, int state){

  Serial.setTimeout(20000L) ;

  // Request Item Label
  //  Serial.println("Input Item Name, ending with #");
  len=Serial.readBytesUntil('#', (char *) buffer, 20) ; // read item from serial
  for (byte i = len; i < 20; i++) buffer[i] = '\s';     // pad with spaces

  blockAddr = 4;
  //Serial.println("Authenticating using key A...");
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddr, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("PCD_Authenticate() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  // Write block
  status = mfrc522.MIFARE_Write(blockAddr, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("MIFARE_Write() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println("MIFARE_Write() success: ");
  state = 0;
}

