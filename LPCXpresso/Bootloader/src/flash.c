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
	SystemCoreClockUpdate();
}

/**
 * Copy 4kB from RAM to flash.
 *
 * @param[in] ramPointer The start of the ram to copy from.
 * @param[in] startSector The sector to start the write from.
 * @return Status of flashing procedure.
 */
flashStatus flashNode( DataBlock *block ) {

	/*
	 * Prepare flash.
	 */
	uint8_t prepare = prepareFlash( block->sector );
	if ( prepare == INVALID_SECTOR) {
		return INVALID_POINTER;
	}
	else if ( prepare == COMPARE_ERROR ) {
		return COMPARE_FAILURE;
	}

	/*
	 * Blank flash.
	 */
	uint8_t blank = blankFlash( block->sector );
	if ( blank == INVALID_SECTOR ) {
		return INVALID_POINTER;
	}
	else if ( prepare == COMPARE_ERROR ) {
		return COMPARE_FAILURE;
	}

	/*
	 * Check if flash sector is blanked.
	 */
	uint8_t checkIfBlank = checkBlank( block->sector );
	if ( checkIfBlank == COMPARE_ERROR ) {
		return COMPARE_FAILURE;
	}
	else if ( checkIfBlank == INVALID_SECTOR ) {
		return INVALID_POINTER;
	}

	/*
	 * Prepare flash.
	 */
	prepare = prepareFlash( block->sector );
	if ( prepare == INVALID_SECTOR) {
		return INVALID_POINTER;
	}
	else if ( prepare == COMPARE_ERROR ) {
		return COMPARE_FAILURE;
	}

	/*
	 * Write 4kB to flash.
	 */
	uint8_t write = writeFlash( block->data, block->sector );
	if ( write == INVALID_SECTOR ) {
		return INVALID_POINTER;
	}
	else if ( write == COMPARE_ERROR ) {
		return COMPARE_FAILURE;
	}

	/*
	 * Compare flashed 4kB.
	 */
	uint8_t compareData = compare( block->data, block->sector );
	if ( compareData == COMPARE_ERROR ) {
		return COMPARE_FAILURE;
	}
	else if ( compare == INVALID_SECTOR ){
		return INVALID_POINTER;
	}

	return FLASH_SUCCESS;
}
