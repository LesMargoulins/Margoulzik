#ifndef MODE_H_
#define MODE_H_

#define DEBUG_MODE 0
#define PIANO_COMPOSE_MODE 1
#define PIANO_PLAY_MODE 2

byte arrayUp[8] = {
  B00100,
  B01110,
  B11111,
  B00100,
  B00100,
  B00100,
  B00000,
};

byte black[8] = {
  B11111,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B11111,
};

byte white[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
};

byte silent[8] = {
  B11111,
  B00000,
  B11111,
  B00000,
  B11111,
  B00000,
  B11111,
};

byte playnote[8] = {
  B00000,
  B00111,
  B00110,
  B00100,
  B00100,
  B11100,
  B11100,
  B11100,
};

byte undo[8] = {
  B00100,
  B01100,
  B11111,
  B01101,
  B00101,
  B10001,
  B11111,
};

byte clearIcon[8] = {
  B00000,
  B10001,
  B01010,
  B00100,
  B01010,
  B10001,
  B00000,
};

byte stopIcon[8] = {
  B00000,
  B00000,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B00000,
};

byte loopable[8] = {
  B00000,
  B01000,
  B01000,
  B01110,
  B00000,
  B00000,
  B00000,
  B00000,
};

byte loopableOn[8] = {
  B00000,
  B01000,
  B01000,
  B01110,
  B00000,
  B01010,
  B00100,
  B00000,
};

/*byte test[] = {
  0x02, 0x6E, 0x10, 0x02, 0x93, 0x10, 0x02, 0xE4, 0x10, 0x00, 0x00, 0x10, 0x03, 0xDC, 0x10, 0x02, 0x93, 0x10, 0x02, 0x6E, 0x10, 0x02, 0x93, 0x10, 0x02, 0xE4, 0x10, 0x03, 0xDC, 0x10, 0x04, 0xDD, 0x10, 0x05, 0x27, 0x10, 0x04, 0xDD, 0x10, 0x03, 0xA4, 0x10, 0x03, 0xDC, 0x10
};*/

#define MusicLen 100
//#define MusicLen 240

struct Note {
  byte note[2] = {
    0x00,
    0x00
  };
  byte duration = 0x00;
};

#endif /* MODE_H_ */
