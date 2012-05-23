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
 * The functions to reset the node with the watchdog timer.
 *
 * @author Chiel de Roest <M.A.deRoest@student.tudelft.nl> and Harmjan Treep <harmjan.treep@gmail.com>
 */

#include <stdint.h>
#include "watchdog.h"
#include "LPC17xx.h"

/**
 * Initialize the watchdog timer.
 *
 * @param[in] time The amount of watchdog clock cycles before
 *                 the processor should reset.
 */
void initWatchdog( uint32_t time ) {
	LPC_WDT->WDTC = time;      // Set the timeout
	LPC_WDT->WDMOD |= (1<<0) | // Enable the watchdog timer
	                  (1<<1);  // Reset the processor when the watchdog timer fires

	watchdogFeed();            // Feed once to enable the watchdog timer
}

/**
 * Reset the watchdog timer.
 */
void watchdogFeed( void ) {
	LPC_WDT->WDFEED = 0xAA;
	LPC_WDT->WDFEED = 0x55;
}

/**
 * Reset the processor via the watchdog timer.
 */
void reset( void ) {
	initWatchdog( 0xFF ); // Initialize the watchdog timer on the smallest
	                      // allowed timeout
	while(1);
}
