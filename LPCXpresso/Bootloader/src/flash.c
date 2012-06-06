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
 * The functions to deal with the flash memory.
 *
 * @author Chiel de Roest <M.A.deRoest@student.tudelft.nl> and Harmjan Treep <harmjan.treep@gmail.com>
 */

#include "flash.h"
#include "iap.h"

/**
 * Initialize the flash memory
 */
void initFlash( void ) {

}

/**
 * Deinitialize the flash memory
 */
void deInitFlash( void ) {

}

/**
 * Returns the physical sector number from the 'virtual' sector number.
 * @param[in] virtualSector The virtual sector number, range [0..128].
 * @return                  The physical sector number, range [0..29].
 */
static uint8_t getPhysicalSectorNumber(uint8_t virtualSector) {

	if ( virtualSector >= 16 ) {
		return ((virtualSector - 16) / 8) + 16;
	}
	return virtualSector;

}

/**
 * Returns the offset of the mapping of a virtual to a physical sector.
 * @param[in] virtualSector The virtual sector.
 * @return        			The offset, range [0..7].
 */
static uint8_t getPhysicalSectorOffset(uint8_t virtualSector) {

	if ( virtualSector < 16 ) {
		return 0;
	}
	else {
		return virtualSector % 8;
	}

}

/**
 * Copy 4kB from RAM to flash.
 *
 * @param[in] ramPointer The start of the ram to copy from.
 * @param[in] startSector The sector to start the write from.
 * @return Status of flashing procedure.
 */
flashStatus flashNode( DataBlock *block ) {

	uint8_t phySector = getPhysicalSectorNumber( block->sector );
	uint8_t offset = getPhysicalSectorOffset( block->sector );

	/*
	 * Prepare flash.
	 */
	uint8_t prepare = prepareFlash( phySector );
	if ( prepare == INVALID_SECTOR) {
		return INVALID_POINTER;
	}
	else if ( prepare == COMPARE_ERROR ) {
		return COMPARE_FAILURE;
	}

	/**
	 * Blank sector only if it is the first 4kB chunck to be flashed on that sector.
	 */
	if ( offset == 0 ) {

		/*
		 * Blank flash.
		 */
		uint8_t blank = blankFlash( phySector );
		if ( blank == INVALID_SECTOR ) {
			return INVALID_POINTER;
		}
		else if ( prepare == COMPARE_ERROR ) {
			return COMPARE_FAILURE;
		}

		/*
		 * Check if flash sector is blanked.
		 */
		uint8_t checkIfBlank = checkBlank( phySector );
		if ( checkIfBlank == COMPARE_ERROR ) {
			return COMPARE_FAILURE;
		}
		else if ( checkIfBlank == INVALID_SECTOR ) {
			return INVALID_POINTER;
		}

	}

	/*
	 * Prepare flash.
	 */
	prepare = prepareFlash( phySector );
	if ( prepare == INVALID_SECTOR) {
		return INVALID_POINTER;
	}
	else if ( prepare == COMPARE_ERROR ) {
		return COMPARE_FAILURE;
	}

	/*
	 * Write 4kB to flash.
	 */
	uint8_t write = writeFlash( block->data, phySector, offset * 4096 );
	if ( write == INVALID_SECTOR ) {
		return INVALID_POINTER;
	}
	else if ( write == COMPARE_ERROR ) {
		return COMPARE_FAILURE;
	}

	/*
	 * Compare flashed 4kB.
	 */
	uint8_t compareData = compareFlash( block->data, phySector, offset * 4096 );
	if ( compareData == COMPARE_ERROR ) {
		return COMPARE_FAILURE;
	}
	else if ( compareData == INVALID_SECTOR ){
		return INVALID_POINTER;
	}

	return FLASH_SUCCESS;

}
