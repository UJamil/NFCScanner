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
  int intByte;

  intByte = Serial.read();
  
  // Authenticate using key A
  Serial.println("Authenticating using key A...");
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("PCD_Authenticate() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  // Show the whole sector as it currently is
  Serial.println("Current data in sector:");
  mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
  Serial.println();

//  if(intByte == "1"){
//    writeNFC (blockAddr, buffer, len, status);
//  }
//  else{  
//    readNFC (blockAddr, buffer, size, status);
//  }
//  // Halt PICC
//  mfrc522.PICC_HaltA();
//  // Stop encryption on PCD
//  mfrc522.PCD_StopCrypto1();
//
//  delay(500);
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

// Read the NFC Card
void readNFC(byte blockAddr, byte *buffer, byte size, byte status){
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
}

//Write to the NFC Card
void writeNFC(byte blockAddr, byte *buffer, byte len, byte status){

  // Ask personal data: First name
  Serial.println("Type First name, ending with #");
  len=Serial.readBytesUntil('#', (char *) buffer, 20) ; // read first name from serial
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

}



