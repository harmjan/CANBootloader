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
	 * TODO: check if writing in bootloader code
	 */

	if ( prepareFlash(block->sector ) != 0 ) {
		return INVALID_POINTER;
	}

	if ( blankFlash( block->sector ) != 0 ) {
		return INVALID_POINTER; // TODO: more specific status
	}

	if ( checkBlank( block->sector ) != 0 ) {
		return INVALID_POINTER; // TODO: more specific status
	}

	if ( prepareFlash(block->sector ) != 0 ) {
		return INVALID_POINTER;
	}

	if ( writeFlash( block->data, block->sector ) != 0 ) {
		return INVALID_POINTER; // TODO: more specific status
	}

	if ( compare( block->data, block->sector ) != 0 ) {
		return COMPARE_FAILURE;
	}

	return FLASH_SUCCESS;
}
