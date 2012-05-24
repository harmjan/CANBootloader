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

#include <stdint.h>

#ifndef FLASH_H__
#define FLASH_H__

/**
 * The status of a flash action.
 */
typedef enum {
	FLASH_SUCCESS     =  0,  /** No errors. */
	COMPARE_FAILURE   = -1, /** Flashing went wrong, after flashing the compare failed so the flash is probably broken. */
	BOOTLOADER_SECTOR = -2, /** Tried to flash the bootloader sector. */
	INVALID_POINTER   = -3  /** One of the pointer is invalid or the region to be copied is invalid. */
} flashStatus;

typedef struct {
	uint8_t data[4096]; // TODO: IAP will not work if data is not word-aligned. Use linker script.
	uint8_t sector;
} DataBlock;

void initFlash( void );
flashStatus flashNode( DataBlock *block );

#endif
