/**
 *     ______       _       ____  _____   ______                    _   __                       __
 *   .' ___  |     / \     |_   \|_   _| |_   _ \                  / |_[  |                     |  ]
 *  / .'   \_|    / _ \      |   \ | |     | |_) |   .--.    .--. `| |-'| |  .--.   ,--.    .--.| | .---.  _ .--.
 *  | |          / ___ \     | |\ \| |     |  __'. / .'`\ \/ .'`\ \| |  | |/ .'`\ \`'_\ : / /'`\' |/ /__\\[ `/'`\]
 *  \ `.___.'\ _/ /   \ \_  _| |_\   |_   _| |__) || \__. || \__. || |, | || \__. |// | |,| \__/  || \__., | |
 *   `.____ .'|____| |____||_____|\____| |_______/  '.__.'  '.__.' \__/[___]'.__.' \'-;__/ '.__.;__]'.__.'[___]
 *
 * ===============================================================================================================
 *
 * The functions to deal with the storage of the start and stack pointers.
 *
 * @author Chiel de Roest <M.A.deRoest@student.tudelft.nl> and Harmjan Treep <harmjan.treep@gmail.com>
 */

#include "storage.h"
#include "eeprom.h"

#define MC24LC64_DATA_ADDR  0x00		/* Address of start of data on the 24LC64 EEPROM */

void initStorage( void ) {
	initEeprom();
}

void savePointersStorage( uint32_t startpointer, uint32_t stackpointer ) {

	uint8_t i, y;

	for ( i = 0; i < 32; i += 8 ) {
		saveByte( MC24LC64_DATA_ADDR + i, (startpointer >> ( 24 - i ) ) & 0xff );
	}

	for ( y = 32; y < 64; y += 8 ) {
		saveByte( MC24LC64_DATA_ADDR + y, (stackpointer >> ( 56 - y ) ) & 0xff );
	}

}

uint32_t getStartPointerStorage( void ) {

	uint32_t startPointer = 0;
	uint8_t i;

	for ( i = 0; i < 32; i += 8 ) {
		startPointer += getByte( MC24LC64_DATA_ADDR + i ) << ( 24 - i );
	}

	return startPointer;
}

uint32_t getStackPointerStorage( void ) {

	uint32_t stackPointer = 0;
	uint8_t i;

	for ( i = 32; i < 64; i += 8 ) {
		stackPointer += (getByte( MC24LC64_DATA_ADDR + i ) & 0xffff) << ( 56 - i );
	}

	return stackPointer;

}
