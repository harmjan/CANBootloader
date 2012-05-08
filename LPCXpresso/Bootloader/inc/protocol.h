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
 * The implementation of the CAN protocol for the bootloader.
 *
 * @author Chiel de Roest <M.A.deRoest@student.tudelft.nl> and Harmjan Treep <harmjan.treep@gmail.com>
 */

#include <stdint.h>

#include "flash.h"

#ifndef PROTOCOL_H__
#define PROTOCOL_H__

/**
 * The statuses that the protocol can communicate to the main function.
 */
typedef enum {
	NO_ACTION,  /** No action needed */
	BOOTLOADER, /** Go into bootloader mode, meaning do not jump to user program */
	DATA_READY, /** 4kB of data have been downloaded and are ready to be flashed */
	RESET_NODE  /** Reset the node  */
} ProtocolState;

void initProtocol( uint8_t *sector, uint8_t *data );
ProtocolState check( void );
void dataStatus( flashStatus state );

#endif
