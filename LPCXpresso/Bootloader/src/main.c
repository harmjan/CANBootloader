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
 * The main entry point for the CAN Bootloader. This bootloader
 * should work for the LPC1769.
 *
 * @author Chiel de Roest <M.A.deRoest@student.tudelft.nl> and Harmjan Treep <harmjan.treep@gmail.com>
 */

#include <stdint.h>

#include "LPC17xx.h"

#include "protocol.h"
#include "flash.h"
#include "storage.h"
#include "timer.h"
#include "watchdog.h"

#include <cr_section_macros.h>

DataBlock block;

ProtocolState state;
uint8_t bootloaderMode = 0; /** If the node is currently in bootloader mode */

/**
 * The main function of the application, the bootloader starts here.
 */
int main(void) {

	// Disable interrupts right from the start
	__disable_irq();

	SystemCoreClockUpdate();

	// Initialize all needed components
	initProtocol( &block );
	initFlash();
	initStorage();
	initTimer();

	// Set the timer for 1 second. If we have not
	// received the signal to go into bootloader
	// mode after 1 second we start the user program.
	timerSet( 1000 );

	while( !timerPassed() || bootloaderMode ) {
		state = check();

		switch( state ) {
		case DATA_READY:

			// If a program is being written into the bootloader code, give error.
			if ( block.sector >= 120 ) {
				dataStatus( BOOTLOADER_SECTOR );
			}
			else if ( block.sector == 0 ) {

				// Save user application's ResetISR pointer and stack pointer.
				uint32_t startPtrUA = ( block.data[7] << 24 ) | ( block.data[6] << 16 ) | ( block.data[5] << 8 ) | ( block.data[4] );
				uint32_t stackPtrUA = ( block.data[3] << 24 ) | ( block.data[2] << 16 ) | ( block.data[1] << 8 ) | ( block.data[0] );
				savePointersStorage( startPtrUA, stackPtrUA );

				// Flash first sector with bootloader's ResetISR pointer and stack pointer,
				// so bootloader is always called first.

				uint8_t *stackptr = (uint8_t *)0x00;
				uint8_t i;
				int32_t checksum = 0;
				for ( i = 0; i < 8; i++ ) {
					block.data[i] = *(stackptr+i);
				}

				uint32_t *index = (uint32_t *)block.data;
				for ( i = 0; i < 7; i++ ) {
					checksum += *(index+i);
				}
				checksum = 0 - checksum;
				block.data[28] =   checksum & 0xff;
				block.data[29] = ( checksum >> 8 ) & 0xff;
				block.data[30] = ( checksum >> 16 ) & 0xff;
				block.data[31] = ( checksum >> 24 ) & 0xff;

				// Flash the node.
				dataStatus( flashNode( &block ) );

			}
			else {
				dataStatus( flashNode( &block ) );
			}

			break;

		case RESET_NODE:
			bootloaderMode = 0;
			//reset();
			break;

		case BOOTLOADER:
			bootloaderMode = 1;
			break;

		case NO_ACTION:
			break;

		}
	}

	uint32_t startPtrUA = getStartPointerStorage();
	uint32_t stackPtrUA = getStackPointerStorage();

	deinitTimer();
	deinitStorage();
	deinitFlash();
	deinitProtocol();

	// Set stack pointer to the start of the user application
	__set_MSP( stackPtrUA );
	__ISB();

	// Force Thumb mode by setting the lowest bit
	startPtrUA |= 0x01;

	// Enable interrupts again for the user application
	__enable_irq();

	// Call the user application's ResetISR routine
	void (*startUA)(void) = (void *)startPtrUA;
	(*startUA)();

	while(1);
	return 0 ;
}
