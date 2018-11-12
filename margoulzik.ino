/*
  Melody

  Plays a melody

  circuit:
  - 8 ohm speaker on digital pin 8

  created 21 Jan 2010
  modified 30 Aug 2011
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Tone
*/

/*
 * MEMORY MAP
 * 
 * 
 * 
 */

#include "pitches.h"
#include "mode.h"
// include the library code:
#include <LiquidCrystal.h>
#include <Encoder.h>
#include <EEPROM.h>

LiquidCrystal lcd(10, 5, 6, 7, 8, 9);
Encoder myEnc(3, 2);

#define KEYBOARD_SIZE 12
#define KEYBOARD_OFFSET 1
#define OCTAVESIZE 7
int keyboard[][KEYBOARD_SIZE] = {
  {NOTE_C1, NOTE_CS1, NOTE_D1, NOTE_DS1, NOTE_E1, NOTE_F1, NOTE_FS1, NOTE_G1, NOTE_GS1, NOTE_A1, NOTE_AS1, NOTE_B1},
  {NOTE_C2, NOTE_CS2, NOTE_D2, NOTE_DS2, NOTE_E2, NOTE_F2, NOTE_FS2, NOTE_G2, NOTE_GS2, NOTE_A2, NOTE_AS2, NOTE_B2},
  {NOTE_C3, NOTE_CS3, NOTE_D3, NOTE_DS3, NOTE_E3, NOTE_F3, NOTE_FS3, NOTE_G3, NOTE_GS3, NOTE_A3, NOTE_AS3, NOTE_B3},
  {NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4},
  {NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_AS5, NOTE_B5},
  {NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_DS6, NOTE_E6, NOTE_F6, NOTE_FS6, NOTE_G6, NOTE_GS6, NOTE_A6, NOTE_AS6, NOTE_B6},
  {NOTE_C7, NOTE_CS7, NOTE_D7, NOTE_DS7, NOTE_E7, NOTE_F7, NOTE_FS7, NOTE_G7, NOTE_GS7, NOTE_A7, NOTE_AS7, NOTE_B7},
};

#define DURATION_SIZE 6

bool error = false;

int currentOctave = 2;

int keyboardColor[] = {
  0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0
};

int duration = 4;

Note music[MusicLen];
int musicCursor = 0;

// constants won't change. They're used here to 
// set pin numbers:
const int buttonPin = 4;     // the number of the pushbutton pin

long oldPosition  = -999;

bool  playingNote = false;
unsigned long   endTime = 0;
unsigned long   millisBeforeEnd = 0;

bool  playingSound = false;
bool  isLoopable = false;
int   soundQueue = 0;

void stopPlay() {
  playingSound = false;
  playingNote = false;
  soundQueue = 0;
  endTime = 0;
  noTone(11);
}

void readNfcBlock(int block, uint8_t success, uint8_t uid[], uint8_t uidLength,  uint8_t keya[], uint8_t *data) {
    success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, block, 0, keya);

    if (success)
    {
      // data = { 'a', 'd', 'a', 'f', 'r', 'u', 'i', 't', '.', 'c', 'o', 'm', 0, 0, 0, 0};
      // success = nfc.mifareclassic_WriteDataBlock (block, data);
  
      success = nfc.mifareclassic_ReadDataBlock(block, data);
    }
    else {
      throwError();
    }
}

void loadFromNfc() {
  // configure board to read RFID tags
  nfc.SAMConfig();

  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  uint8_t data[16];
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    if (uidLength != 4)
      return;
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
      readNfcBlock(1, success, uid, uidLength, keya, data);
      musicCursor = data[0];
      if (musicCursor > MusicLen)
        return;
      
      int j = 0;
      int noteid = 0;
      int innoteid = 0;
      for (int i = 4; j < musicCursor * 3; i++) {
        if ((i - 4) % 4 == 3)
          continue;
        readNfcBlock(i, success, uid, uidLength, keya, data);
        for (int w = 0; w < 16 && noteid < musicCursor; w++) {
          if (innoteid == 0)
            music[noteid].note[0] = data[w];
          else if (innoteid == 1)
            music[noteid].note[1] = data[w];
          else
            music[noteid].duration = data[w];
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

void writeNfcBlock(int block, uint8_t success, uint8_t uid[], uint8_t uidLength,  uint8_t keya[], uint8_t *data) {
    success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, block, 0, keya);

    if (success)
    {
      // data = { 'a', 'd', 'a', 'f', 'r', 'u', 'i', 't', '.', 'c', 'o', 'm', 0, 0, 0, 0};
      success = nfc.mifareclassic_WriteDataBlock (block, data);
    }
    else {
      throwError();
    }
}

void saveToNfc() {
  // configure board to read RFID tags
  nfc.SAMConfig();

  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  uint8_t data[16] = {musicCursor, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    if (uidLength != 4)
      return;
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
      writeNfcBlock(1, success, uid, uidLength, keya, data);
      if (musicCursor > MusicLen)
        return;
      Serial.print("Going to save ");
      Serial.println(musicCursor);
      int j = 0;
      int noteid = 0;
      int innoteid = 0;
      for (int i = 4; j < musicCursor * 3; i++) {
        if ((i - 4) % 4 == 3)
          continue;
        for (int w = 0; w < 16; w++) {
          if (noteid >= musicCursor)
            data[w] = 0;
          else if (innoteid == 0)
            data[w] = music[noteid].note[0];
          else if (innoteid == 1)
            data[w] = music[noteid].note[1];
          else
            data[w] = music[noteid].duration;
          innoteid += 1;
          if (innoteid >= 3) {
            innoteid = 0;
            noteid += 1;
          }
        }
        nfc.PrintHexChar(data, 16);
        writeNfcBlock(i, success, uid, uidLength, keya, data);
        j += 16;
      }
  }
  else {
    throwError();
  }
}

bool playNote(int note, int duration, bool force = false) {
    if (playingNote) {
      if (force) {
        stopPlay();
        delay(10);
      }
      else {
        return false;
      }
    }
    playingNote = true;
    int noteDuration = 1000 / duration;
    tone(11, note, noteDuration);
    unsigned long m = millis();

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    unsigned long pauseBetweenNotes = noteDuration * 1.00;
    endTime = pauseBetweenNotes;
    millisBeforeEnd = m;
    return true;
}

void NoteListener() {
  if (playingNote == true && millis() > millisBeforeEnd + endTime) {
    playingNote = false;
    noTone(11);
  }
  if (!playingNote) {
    if (playingSound == true && soundQueue < musicCursor) {
      int currentnote = music[soundQueue].note[1] << 8;
      currentnote += music[soundQueue].note[0];
      playNote(currentnote, music[soundQueue].duration);
      soundQueue += 1;
    }
  }
  if (playingSound && soundQueue >= musicCursor) {
    soundQueue = 0;
    if (!isLoopable)
      playingSound = false;
  }
}

void playMusic() {
  if (playingSound) {
    Serial.println("Already playing music");
    return;
  }
  playingSound = true;
  soundQueue = 0;
}

void loadMusic(byte *ar, int len) {
  musicCursor = len;
  for (int i = 0; i < len; i++) {
    music[i].note[0] = ar[(i * 3)];
    music[i].note[1] = ar[(i * 3) + 1];
    music[i].duration = ar[(i * 3) + 2];
  }
}

void setup() {
  lcd.clear();
  Serial.begin(9600);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print(F("Booting..."));

  nfc.begin();
    
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print(F("Didn't find PN53x board"));
    return;
  }

  
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);  
  lcd.createChar(0, arrayUp);
  lcd.createChar(1, black);
  lcd.createChar(2, white);
  lcd.createChar(3, silent);
  drawKeyboard();
}

void saveToEeprom() {
  EEPROM.write(0, musicCursor);
  for (int i = 0; i < musicCursor; i++) {
    EEPROM.write((i * 3) + 1, music[i].note[0]);
    EEPROM.write((i * 3) + 2, music[i].note[1]);
    EEPROM.write((i * 3) + 3, music[i].duration);
  }
}

void loadFromEeprom() {
  musicCursor = EEPROM.read(0);
  for (int i = 0; i < musicCursor; i++) {
    music[i].note[0] = EEPROM.read((i * 3) + 1);
    music[i].note[1] = EEPROM.read((i * 3) + 2);
    music[i].duration = EEPROM.read((i * 3) + 3);
  }
}

void clearEeprom() {
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
}

void throwError() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("An error occured");
  error = true;
}

void dumpEeprom() {
  Serial.print(F("Eeprom: "));
  Serial.println(EEPROM.length());
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    Serial.println(EEPROM.read(i), DEC);
  }
}

int mode = PIANO_COMPOSE_MODE;
bool clicked = false;
bool releasedClick = false;
bool isClicked = false;
int lastButtonState = 0;
// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status
unsigned long lastmillis = 0;

void loop() {
  buttonState = digitalRead(buttonPin);

  isClicked = (buttonState != LOW);
  if (lastButtonState != buttonState && millis() - lastmillis >= 100) {
    if (buttonState == HIGH) {
      releasedClick = true;
    } 
    else {
      clicked = true;
    }
    lastButtonState = buttonState;
    lastmillis = millis();
  }

  if (error) {
    if (clicked == true) {
      lcd.clear();
      drawParam();
      error = false;
    }
  }
  else {
    switch (mode) {
      case DEBUG_MODE:
        debugMode();
        break;
      case PIANO_COMPOSE_MODE:
        composeMode();
        break;
    }
    NoteListener();
  }
  delay(10);
  clicked = false;
  releasedClick = false;
}

int composeModePage = 0;

int intLen(int i) {
  return (floor(log10(abs(i))) + 1);
}

void addNote(int playednote) {
    if (musicCursor < MusicLen) {
      music[musicCursor].note[0] = playednote;
      music[musicCursor].note[1] = playednote >> 8;
      music[musicCursor].duration = duration;
      musicCursor += 1;
    }
    lcd.setCursor(16 - intLen(musicCursor), 1);
    lcd.print(musicCursor);
}


void drawKeyboard() {
  lcd.setCursor(0, 0);
  if (currentOctave > 0) {
    lcd.print(F("<"));
  }
  else {
    lcd.print(F(" "));
  }

  for (int i = KEYBOARD_OFFSET; i < KEYBOARD_SIZE + KEYBOARD_OFFSET; i++) {
    lcd.setCursor(i, 0);
    if (keyboardColor[i - KEYBOARD_OFFSET])
      lcd.write(byte(1));
     else
      lcd.write(byte(2));
  }
  lcd.setCursor(13, 0);
  lcd.write(byte(3));
  lcd.setCursor(14, 0);
  if (currentOctave < 6) {
    lcd.print(F(">"));
  }
  else {
    lcd.print(F(" "));
  }
  lcd.setCursor(15, 0);
  lcd.print(F("+"));
}

void composeModeA(long newPosition) {
  if (newPosition != oldPosition) {
    lcd.setCursor(oldPosition, 1);
    lcd.print(F(" "));
    lcd.setCursor(16 - intLen(musicCursor), 1);
    lcd.print(musicCursor);
    oldPosition = newPosition;
    drawKeyboard();
    lcd.setCursor(newPosition, 1);
    lcd.write(byte(0));
  }

  if (clicked) {
    if (newPosition == 0) {
      if (currentOctave > 0) {
        currentOctave -= 1;
        drawKeyboard();
      }
    }
    else if (newPosition >= KEYBOARD_OFFSET && newPosition < KEYBOARD_SIZE + KEYBOARD_OFFSET) {
      playNote(keyboard[currentOctave][newPosition - KEYBOARD_OFFSET], duration, true);
      addNote(keyboard[currentOctave][newPosition - KEYBOARD_OFFSET]);
    }
    else if (newPosition == 13) {
      addNote(0);
    }
    else if (newPosition == 14) {
      if (currentOctave < 6) {
        currentOctave += 1;
        drawKeyboard();
      }
    }
    else if (newPosition == 15) {
      composeModePage = 1;
      lcd.createChar(1, playnote);
      lcd.createChar(2, isLoopable?loopableOn:loopable);
      lcd.createChar(3, undo);
      lcd.createChar(4, stopIcon);
      lcd.clear();
      oldPosition = -1;
    }
  }
}

void removeNote() {
  if (musicCursor > 0) {
    musicCursor -= 1;
    lcd.setCursor(16 - 3, 1);
    lcd.print(F("   "));
  }
  lcd.setCursor(16 - intLen(musicCursor), 1);
  lcd.print(musicCursor);
}

void drawParam() {
  lcd.setCursor(4, 0);//SPEED v ^
  lcd.print(F("^v"));

  lcd.setCursor(1, 0);//STOP
  lcd.write(byte(4));

  lcd.setCursor(2, 0);
  lcd.write(byte(3));
  lcd.setCursor(0, 0);
  lcd.write(byte(1));
  lcd.setCursor(3, 0);
  lcd.write(byte(2));
  lcd.setCursor(7, 0);
  lcd.print(F("DU SL X"));
  lcd.setCursor(15, 0);
  lcd.print(F("-"));
}


void composeModeB(long newPosition) {
  if (newPosition != oldPosition) {
    lcd.setCursor(oldPosition, 1);
    lcd.print(F(" "));
    lcd.setCursor(0, 1);
    lcd.print("1/  ");
    lcd.setCursor(2, 1);
    lcd.print(duration);
    lcd.setCursor(16 - intLen(musicCursor), 1);
    lcd.print(musicCursor);
    oldPosition = newPosition;
    drawParam();
    lcd.setCursor(newPosition, 1);
    lcd.write(byte(0));
  }
  
  if (clicked) {
    if (newPosition == 0) {
      if (musicCursor > 0)
        playMusic();
    }
    else if (newPosition == 1) {
      stopPlay();
    }
    else if (newPosition == 2) {
      removeNote();
    }
    else if (newPosition == 3) {
      isLoopable = !isLoopable;
      if (!isLoopable)
        lcd.createChar(2, loopable);
      else
        lcd.createChar(2, loopableOn);
      drawParam();
    }
    else if (newPosition == 4) {
      if (duration < 32) {
        duration += 1;
        drawParam();
      }
      lcd.setCursor(0, 1);
      lcd.print("1/  ");
      lcd.setCursor(2, 1);
      lcd.print(duration);
    }
    else if (newPosition == 5) {
      if (duration > 1) {
        duration -= 1;
        drawParam();
      }
      lcd.setCursor(0, 1);
      lcd.print("1/  ");
      lcd.setCursor(2, 1);
      lcd.print(duration);
    }
    else if (newPosition == 7) {
      loadFromNfc();
    }
    else if (newPosition == 8) {
      saveToNfc();
    }
    else if (newPosition == 10) {
      saveToEeprom();
      drawParam();
    }
    else if (newPosition == 11) {
      loadFromEeprom();
      drawParam();
    }
    else if (newPosition == 13) {
      musicCursor = 0;
      lcd.setCursor(16 - 3, 1);
      lcd.print("   ");
    }
    else if (newPosition == 15) {
      composeModePage = 0;
      lcd.createChar(1, black);
      lcd.createChar(2, white);
      lcd.createChar(3, silent);
      lcd.clear();
      oldPosition = -1;
    }
  }
}

void composeMode() {
  if (myEnc.read() < 0)
    myEnc.write(16*4);
  else if (myEnc.read() / 4 >= 16)
    myEnc.write(0);
  long newPosition = myEnc.read() / 4;

  if (composeModePage == 0)
    composeModeA(newPosition);
   else if (composeModePage == 1)
    composeModeB(newPosition);
}

void debugMode() {

  // print cursor position:
  lcd.setCursor(4, 0);
  lcd.print(F("Cursor: "));

  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    lcd.setCursor(12, 0);
    lcd.print(F("    "));
    lcd.setCursor(12, 0);
    lcd.print(newPosition);
  }


  // print the number of seconds since reset:
  lcd.setCursor(0, 1);
  lcd.print(millis() / 1000);
}

