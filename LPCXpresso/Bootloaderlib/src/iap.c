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
 * The functions to deal with the IAP API.
 *
 * @author Chiel de Roest <M.A.deRoest@student.tudelft.nl> and Harmjan Treep <harmjan.treep@gmail.com>
 */

#include "iap.h"

IAP iapFunction=(IAP)IAP_LOCATION;

/**
 * Prepare the flash for writing.
 *
 * @param[in] sector The sector to prepare for writing.
 * @return Error code.
 */
uint8_t prepareFlash( uint8_t sector ) {

	uint8_t prepare = iap_write_prepare( sector, sector );

	if ( prepare == BUSY ) {
		return COMPARE_ERROR;
	}
	else {
		return prepare;
	}

}

/**
 * Blanks the given sector, i.e. fills completely with 1's.
 *
 * @param[in] sector The sector to be blanked.
 * @return Error code.
 */
uint8_t blankFlash( uint8_t sector ) {

	uint8_t blank = iap_erase( sector, sector );

	if ( blank == CMD_SUCCESS || blank == INVALID_SECTOR ) {
		return blank;
	}
	else {
		return COMPARE_ERROR;
	}

}

/**
 * Check if a sector of the flash is blank, meaning that the sector
 * is completely filled with 1's.
 *
 * @param[in] sector The sector to blank check.
 * @return Error code.
 */
uint8_t checkBlank( uint8_t sector ) {

	uint8_t checkIfBlank = iap_blank_check( sector, sector );

	if ( checkIfBlank == CMD_SUCCESS || checkIfBlank == INVALID_SECTOR ) {
		return checkIfBlank;
	}
	else {
		return COMPARE_ERROR;
	}

}

/**
 * Compare a 4kB block of RAM with a block in flash.
 *
 * @param data Pointer to the start of the block in RAM.
 * @param sector The sector where the block in flash starts.
 * @return If the two blocks are the same. If an error was encountered
 *         false is returned.
 */
uint8_t compare( uint8_t *data, uint8_t sector ) {

	uint8_t compare = iap_compare( sector_start_adress[sector], data, 4*1024 );

	switch ( compare ) {

		case CMD_SUCCESS:
			return CMD_SUCCESS;
		break;
		case COMPARE_ERROR:
		case COUNT_ERROR:
			return COMPARE_ERROR;
		break;
		case ADDR_ERROR:
		case ADDR_NOT_MAPPED:
		default:
			return INVALID_SECTOR;
		break;

	}

}

/**
 * Write a block of 4kB of data from the RAM to the flash.
 *
 * Make sure the flash you are going to write to is prepared and
 * cleared if necessary.
 *
 * @param[in] data A pointer to the start of the data in the RAM.
 * @param[in] sector The sector where the write starts.
 */
uint8_t writeFlash( uint8_t *data, uint8_t sector ) {

	uint8_t write = iap_write( data, sector_start_adress[sector], 4*1024 );

	switch ( write ) {

		case CMD_SUCCESS:
			return CMD_SUCCESS;
		break;
		case SRC_ADDR_ERROR:
		case DST_ADDR_ERROR:
		case SRC_ADDR_NOT_MAPPED:
		case DST_ADDR_NOT_MAPPED:
			return INVALID_SECTOR;
		break;
		default:
			return COMPARE_ERROR;
		break;

	}

}

/**
 * Get the unique serial ID of this processor.
 *
 * Every LPC processor has an 128 bit unique ID. This ID is unique
 * over all LPC processors.
 *
 * @param[out] serial The device serial, all 16 bytes of it.
 */
void getDeviceSerial( uint8_t *serial ) {
	DeviceSerial serialStruct;
	iap_read_serial( &serialStruct );

	uint8_t i=0;
	for( i=0; i<4; i++ ) {
		*serial = serialStruct.ser1 & 0xFF;
		serialStruct.ser1 = serialStruct.ser1 >> 8;
		serial++;
	}
	for( i=0; i<4; i++ ) {
		*serial = serialStruct.ser2 & 0xFF;
		serialStruct.ser2 = serialStruct.ser2 >> 8;
		serial++;
	}
	for( i=0; i<4; i++ ) {
		*serial = serialStruct.ser3 & 0xFF;
		serialStruct.ser3 = serialStruct.ser3 >> 8;
		serial++;
	}
	for( i=0; i<4; i++ ) {
		*serial = serialStruct.ser4 & 0xFF;
		serialStruct.ser4 = serialStruct.ser4 >> 8;
		serial++;
	}
}
