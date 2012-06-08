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
 * The functions to deal with talking to the onboard eeprom chip on the LPCXpresso.
 *
 * @author Chiel de Roest <M.A.deRoest@student.tudelft.nl> and Harmjan Treep <harmjan.treep@gmail.com>
 */

#include "eeprom.h"
#include "i2c.h"

#define MC24LC64_SLAVE_ADDR 0xa0		/* I2C address of the 24LC64 EEPROM */

/**
 * Initialize the EEPROM peripheral.
 */
void initEeprom( void ) {
	initI2C( MC24LC64_SLAVE_ADDR );
}

/**
 * Deinitialize the EEPROM peripheral.
 */
void deinitEeprom( void ) {
	deinitI2C();
}

/**
 * Saves the byte of data at location 'address' in EEPROM.
 * @param[in] address The address for the EEPROM to save the data to.
 * @param[in] byte    The data to save.
 */
void eepromSaveByte( uint16_t address, uint8_t byte ) {

	int8_t temp = i2cSend( MC24LC64_SLAVE_ADDR, address, byte );

	/**
	 * TODO: error handling
	 */

}

/**
 * Retrieves the byte stored in the EEPROM at the given address.
 * @param[in] address Address of the byte in the EEPROM.
 * @return            The byte in the EEPROM at the location of the given address.
 */
uint8_t eepromGetByte( uint16_t address ) {

	return i2cReceive( MC24LC64_SLAVE_ADDR, address );

}
