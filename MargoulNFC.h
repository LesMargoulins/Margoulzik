#ifndef MARGOULNFC_H_
  #define MARGOULNFC_H_
  #include <Arduino.h>
  #include <Wire.h>
  #include <PN532_I2C.h>
  #include <PN532.h>
  #include "mode.h"

  class MargoulNFC {
    private:
      PN532 _nfc;
    public:
      MargoulNFC();
      ~MargoulNFC();
      void readNfcBlock(int block, uint8_t success, uint8_t uid[], uint8_t uidLength,  uint8_t keya[], uint8_t *data);
      void loadFromNfc(struct Note *music[100], int *musicCursor);
      void writeNfcBlock(int block, uint8_t success, uint8_t uid[], uint8_t uidLength,  uint8_t keya[], uint8_t *data);
      void saveToNfc(struct Note *music[100], int *musicCursor);

  };


#endif /* MARGOULNFC_H_ */
