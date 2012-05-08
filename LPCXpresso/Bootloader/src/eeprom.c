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
 * The functions to deal with taling to the onboard eeprom chip on the LPCXpresso.
 *
 * @author Chiel de Roest <M.A.deRoest@student.tudelft.nl> and Harmjan Treep <harmjan.treep@gmail.com>
 */

#include "eeprom.h"

#define MC24LC64_SLAVE_ADDR 0xa0		/* I2C address of the 24LC64 EEPROM */

/**
 * Initialize the EEPROM peripheral.
 */
void initEeprom( void ) {
	initI2C( MC24LC64_SLAVE_ADDR );
}

/**
 * Saves the byte of data at location 'address' in EEPROM.
 * @param[in] address The address for the EEPROM to save the data to.
 * @param[in] byte    The data to save.
 */
void saveByte( uint16_t address, uint8_t byte ) {

	send( ( address >> 8 ) & 0xff );
	send( address & 0xff );
	send( byte );

}

/**
 * Retrieves the byte stored in the EEPROM at the given address.
 * @param[in] address Address of the byte in the EEPROM.
 * @return            The byte in the EEPROM at the location of the given address.
 */
uint8_t getByte( uint16_t address ) {

	return retrieve( address );

}
