#ifndef MMODE_H_
	#define MMODE_H_

	#define DEBUG_MODE 0
	#define PIANO_COMPOSE_MODE 1
	#define PIANO_PLAY_MODE 2

	#define MusicLen 100
	//#define MusicLen 240

	struct Note {
	  byte note[2] = {
		0x00,
		0x00
	  };
	  byte duration = 0x00;
	};

#endif /* MMODE_H_ */
