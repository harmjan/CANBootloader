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

#define MC24LC64_DATA_ADDR  0x0000		/* Address of start of data on the 24LC64 EEPROM */

/**
 * Initialize the storage unit.
 */
void initStorage( void ) {
	initEeprom();
}

/**
 * Deinitialize the storage unit.
 */
void deinitStorage( void ) {
	deInitEeprom();
}

/**
 * Save both the start and the stack pointer in the storage unit at the predefined address.
 * @param[in] startpointer The value of the startpointer.
 * @param[in] stackpointer The value of the stackpointer.
 */
void savePointersStorage( uint32_t startpointer, uint32_t stackpointer ) {

	uint8_t i, y;

	for ( i = 0; i < 4; i++ ) {
		eepromSaveByte( MC24LC64_DATA_ADDR + i, (startpointer >> ( 8*i ) ) & 0xff );
	}

	for ( y = 0; y < 4; y++ ) {
		eepromSaveByte( MC24LC64_DATA_ADDR + y + 4, (stackpointer >> ( 8*y ) ) & 0xff );
	}

}

/**
 * Retrieve the start pointer from the storage unit.
 * @return The value of the start pointer.
 */
uint32_t getStartPointerStorage( void ) {

	uint32_t startPointer = 0;
	uint8_t i;

	for ( i = 0; i < 4; i++ ) {
		startPointer += ( eepromGetByte( MC24LC64_DATA_ADDR + i ) & 0xffff ) << ( 8*i );
	}

	return startPointer;
}

/**
 * Retrieve the stack pointer from the storage unit.
 * @return The value of the stack pointer.
 */
uint32_t getStackPointerStorage( void ) {

	uint32_t stackPointer = 0;
	uint8_t i;

	for ( i = 0; i < 4; i++ ) {
		stackPointer += ( eepromGetByte( MC24LC64_DATA_ADDR + i + 4 ) & 0xffff) << ( 8*i );
	}

	return stackPointer;

}
