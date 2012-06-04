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
				uint32_t startPtrUA = ( block.data[3] << 24 ) | ( block.data[2] << 16 ) | ( block.data[1] << 8 ) | ( block.data[0] );
				uint32_t stackPtrUA = ( block.data[7] << 24 ) | ( block.data[6] << 16 ) | ( block.data[5] << 8 ) | ( block.data[4] );
				savePointersStorage( startPtrUA, stackPtrUA );

				// Flash first sector with bootloader's ResetISR pointer and stack pointer,
				// so bootloader is always called first.
				uint32_t *startPtrBL = (uint32_t *) 0x04;
				uint32_t *stackPtrBL = (uint32_t *) 0x00;
				block.data[0] =   *startPtrBL         & 0xff;
				block.data[1] = ( *startPtrBL >> 8 )  & 0xff;
				block.data[2] = ( *startPtrBL >> 16 ) & 0xff;
				block.data[3] = ( *startPtrBL >> 24 ) & 0xff;
				block.data[4] =   *stackPtrBL 		  & 0xff;
				block.data[5] = ( *stackPtrBL >> 8 )  & 0xff;
				block.data[6] = ( *stackPtrBL >> 16 ) & 0xff;
				block.data[7] = ( *stackPtrBL >> 24 ) & 0xff;

				// Flash the node.
				dataStatus( flashNode( &block ) );

			}
			else {
				dataStatus( flashNode( &block ) );
			}

			break;

		case RESET_NODE:
			reset();
			break;

		case BOOTLOADER:
			bootloaderMode = 1;
			break;

		case NO_ACTION:
			break;

		}
	}

	// Enable interrupts again for the user application
	__enable_irq();

	uint32_t startPtrUA = getStartPointerStorage();
	uint32_t stackPtrUA = getStackPointerStorage();

	// Set stack pointer to the start of the user application
	__set_MSP( stackPtrUA );

	// Force Thumb mode by setting the lowest bit
	startPtrUA |= 0x01;

	// Call the user application's ResetISR routine
	void (*startUA)(void) = (void *)startPtrUA;
	(*startUA)();

	while(1);
	return 0 ;
}
