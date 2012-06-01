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

/** The place to store the received data and to read from when flash the node. */
static DataBlock block;

/** The boolean to save if this node is currently in bootloading mode. */
static uint8_t bootloaderMode = 0;

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
	timerSet( 10000 );

	while( !timerPassed() || bootloaderMode ) {
		ProtocolState state = check();

		switch( state ) {
		case DATA_READY:
			dataStatus( FLASH_SUCCESS ); // TODO Really flash the node
			// TODO Check if it is the bootloader sector, if it is do not flash! Give error!
			/*if( block.sector == 0 ) {
				dataStatus( BOOTLOADER_SECTOR );
			} else {
				dataStatus( flashNode( &theBlock ) );
			}*/
			break;

		case RESET_NODE:
			reset();
			break;

		case BOOTLOADER:
			bootloaderMode = 1;
			break;
		}
	}

	// Enable interrupts again for the user application
	__enable_irq();

	// TODO Jump to user application

	while(1);
	return 0 ;
}
