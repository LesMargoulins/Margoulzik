#ifndef MARGOULEEPROM_H_
	#define MARGOULEEPROM_H_
	
	#include <Arduino.h>
	#include <EEPROM.h>
	//#include <Serial.h>
	#include "mode.h"

class MargoulEeprom {
		public:
			MargoulEeprom();
			void saveToEeprom(struct Note music[100], int musicCursor);
			void loadFromEeprom(struct Note music[100], int *musicCursor);
			void clearEeprom();
			void dumpEeprom();
	};

#endif /* MARGOULEEPROM_H_ */