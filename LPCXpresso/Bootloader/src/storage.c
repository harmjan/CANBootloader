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
 * The functions to deal with the storage of the start and stack pointers.
 *
 * @author Chiel de Roest <M.A.deRoest@student.tudelft.nl> and Harmjan Treep <harmjan.treep@gmail.com>
 */

#include "storage.h"

/**
 * Initialize the storage unit.
 */
void initStorage( void ) {
}

/**
 * Deinitialize the storage unit.
 */
void deinitStorage( void ) {
}

/**
 * Save both the start and the stack pointer in the storage unit at the predefined address.
 * @param[in] startpointer The value of the startpointer.
 * @param[in] stackpointer The value of the stackpointer.
 */
void savePointersStorage( uint32_t startpointer, uint32_t stackpointer ) {

	*((uint32_t *)0x20) = stackpointer;
	*((uint32_t *)0x24) = startpointer;

}

/**
 * Retrieve the start pointer from the storage unit.
 * @return The value of the start pointer.
 */
uint32_t getStartPointerStorage( void ) {

	return *((uint32_t *)0x24);

}

/**
 * Retrieve the stack pointer from the storage unit.
 * @return The value of the stack pointer.
 */
uint32_t getStackPointerStorage( void ) {

	return *((uint32_t *)0x20);

}
