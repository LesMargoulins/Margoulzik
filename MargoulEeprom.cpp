#include "MargoulEeprom.h"

MargoulEeprom::MargoulEeprom() {
	Serial.begin(9600);
	Serial.println("constructeur");
}

void MargoulEeprom::saveToEeprom(struct Note music[100], int musicCursor) {
	Serial.println("Save To Eeprom");

	EEPROM.write(0, musicCursor);
	for (int i = 0; i < musicCursor; i++) {
		EEPROM.write((i * 3) + 1, music[i].note[0]);
		EEPROM.write((i * 3) + 2, music[i].note[1]);
		EEPROM.write((i * 3) + 3, music[i].duration);
	}
}

void MargoulEeprom::loadFromEeprom(struct Note music[100], int *musicCursor) {
	Serial.println(*musicCursor);
	*musicCursor = EEPROM.read(0);
	Serial.println(*musicCursor);
	for (int i = 0; i < *musicCursor; i++) {
		music[i].note[0] = EEPROM.read((i * 3) + 1);
		music[i].note[1] = EEPROM.read((i * 3) + 2);
		music[i].duration = EEPROM.read((i * 3) + 3);
	}
}

void MargoulEeprom::clearEeprom() {
	for (int i = 0; i < EEPROM.length(); i++) {
		EEPROM.write(i, 0);
	}
}

void MargoulEeprom::dumpEeprom() {
	Serial.print(F("Eeprom: "));
	Serial.println(EEPROM.length());
	for (int i = 0; i < EEPROM.length(); i++) {
		Serial.println(EEPROM.read(i), DEC);
	}
}