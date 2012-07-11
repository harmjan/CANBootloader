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
 * The main entry point for the CAN Bootloader programmer.
 * 1 node in the network should run this software to update
 * all other nodes in the network.
 *
 * @author Chiel de Roest <M.A.deRoest@student.tudelft.nl> and Harmjan Treep <harmjan.treep@gmail.com>
 */

#include "LPC17xx.h"
#include "protocol.h"
#include "timer.h"
#include "host.h"

/**
 * The list of nodes
 */
nodelist list;

extern uint8_t _binary_userapplication_bin_start;
extern uint8_t _binary_userapplication_bin_end;
extern uint8_t _binary_userapplication_bin_size;

int main( void ) {

	initHost();
	initProtocol();
	SystemCoreClockUpdate();

	for( ;; ) {
		uint8_t command = hostListen();
		switch ( command ) {
		case 0x01: // Scan network
			hostSendResponse( command ); // Send response back to host
			protocolDiscover( &list );   // Scan network
			hostSendData( (uint8_t *)(&(list.numNodes)), sizeof(list.numNodes) ); // Send number of responding nodes
			hostSendData( (uint8_t *)(&(list.ids)), sizeof(list.ids[0]) * list.numNodes ); // Send IDs of all responding nodes
			hostSendResponse( command ); // Send response back to host to mark end of data
			if ( hostListen() != command ) { // Host returns response to confirm success on data transaction
				// Go to error state
				// TODO ^
			}
			break;
		}
	}

	/*SystemCoreClockUpdate();

	initProtocol();

	protocolDiscover( &list );
	protocolProgram( &list,
			&_binary_userapplication_bin_start,
			&_binary_userapplication_bin_end );
	protocolReset();
*/
	while(1);
	return 0;
}
