#include "MargoulNFC.h"

MargoulNFC::MargoulNFC()
{
  PN532_I2C pn532_i2c(Wire);
  PN532 _nfc(pn532_i2c);

  _nfc.begin();

  uint32_t versiondata = _nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print(F("Didn't find PN53x board"));
    return;
  }
}

MargoulNFC::~MargoulNFC()
{

}

void MargoulNFC::readNfcBlock(int block, uint8_t success, uint8_t uid[], uint8_t uidLength,  uint8_t keya[], uint8_t *data) {
    success = _nfc.mifareclassic_AuthenticateBlock(uid, uidLength, block, 0, keya);

    if (success)
    {
      // data = { 'a', 'd', 'a', 'f', 'r', 'u', 'i', 't', '.', 'c', 'o', 'm', 0, 0, 0, 0};
      // success = _nfc.mifareclassic_WriteDataBlock (block, data);

      success = _nfc.mifareclassic_ReadDataBlock(block, data);
    }
    else {
      throwError();
    }
}

void MargoulNFC::loadFromNfc(struct Note *music[100], int *musicCursor) {
  // configure board to read RFID tags
  _nfc.SAMConfig();

  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  uint8_t data[16];
  success = _nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    if (uidLength != 4)
      return;
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
      readNfcBlock(1, success, uid, uidLength, keya, data);
      *musicCursor = data[0];
      if (*musicCursor > MusicLen)
        return;

      int j = 0;
      int noteid = 0;
      int innoteid = 0;
      for (int i = 4; j < *musicCursor * 3; i++) {
        if ((i - 4) % 4 == 3)
          continue;
        readNfcBlock(i, success, uid, uidLength, keya, data);
        for (int w = 0; w < 16 && noteid < *musicCursor; w++) {
          if (innoteid == 0)
            music[noteid]->note[0] = data[w];
          else if (innoteid == 1)
            music[noteid]->note[1] = data[w];
          else
            music[noteid]->duration = data[w];
          innoteid += 1;
          if (innoteid >= 3) {
            innoteid = 0;
            noteid += 1;
          }
        }
        j += 16;
      }
  }
    else {
      throwError();
    }

}

void MargoulNFC::writeNfcBlock(int block, uint8_t success, uint8_t uid[], uint8_t uidLength,  uint8_t keya[], uint8_t *data) {
    success = _nfc.mifareclassic_AuthenticateBlock(uid, uidLength, block, 0, keya);

    if (success)
    {
      // data = { 'a', 'd', 'a', 'f', 'r', 'u', 'i', 't', '.', 'c', 'o', 'm', 0, 0, 0, 0};
      success = _nfc.mifareclassic_WriteDataBlock (block, data);
    }
    else {
      throwError();
    }
}

void MargoulNFC::saveToNfc(struct Note *music[100], int *musicCursor) {
  // configure board to read RFID tags
  _nfc.SAMConfig();

  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  uint8_t data[16] = {*musicCursor, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  success = _nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    if (uidLength != 4)
      return;
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
      writeNfcBlock(1, success, uid, uidLength, keya, data);
      if (*musicCursor > MusicLen)
        return;
      Serial.print("Going to save ");
      Serial.println(*musicCursor);
      int j = 0;
      int noteid = 0;
      int innoteid = 0;
      for (int i = 4; j < *musicCursor * 3; i++) {
        if ((i - 4) % 4 == 3)
          continue;
        for (int w = 0; w < 16; w++) {
          if (noteid >= *musicCursor)
            data[w] = 0;
          else if (innoteid == 0)
            data[w] = music[noteid]->note[0];
          else if (innoteid == 1)
            data[w] = music[noteid]->note[1];
          else
            data[w] = music[noteid]->duration;
          innoteid += 1;
          if (innoteid >= 3) {
            innoteid = 0;
            noteid += 1;
          }
        }
        _nfc.PrintHexChar(data, 16);
        writeNfcBlock(i, success, uid, uidLength, keya, data);
        j += 16;
      }
  }
  else {
    throwError();
  }
}
