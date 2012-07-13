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
 * The implementation of the CAN protocol for the programmer.
 *
 * @author Chiel de Roest <M.A.deRoest@student.tudelft.nl> and Harmjan Treep <harmjan.treep@gmail.com>
 */

#include <stdint.h>

#include "protocol.h"
#include "can.h"
#include "timer.h"
#include "hash.h"

/** The temporary message object */
static CanMessage msg;

/** The temporary block object */
static DataBlock block;

/** Are the nodes to be reprogrammed already selected */
static uint8_t selected = 0;

static void selectNodes( nodelist *list );

/**
 * Write 4kB of data to the nodes.
 * @param list The list of nodes that the
 *             block should be written to
 * @param block The block of data to write.
 * @return If the writing was succesfull
 */
static uint8_t writeBlock( nodelist *list, DataBlock *block ) {

	// Send the sector where the following 4kB of data
	// should be put
	msg.id      = 0x104;
	msg.length  = 1;
	msg.data[0] = block->sector;
	canSend( &msg );

	// Send the 4kB of data
	uint8_t *index = block->data;
	msg.id         = 0x105;
	msg.length     = 8;
	initHash();
	{
		uint16_t i;
		for( i=0; i<512; i++ ) {
			uint8_t j;
			for( j=0; j<8; j++ ) {
				msg.data[j] = *(index);
				++index;
			}
			canSend( &msg );
			hashUpdate(msg.data);
		}
	}

	// Send the hash of the data
	msg.id      = 0x106;
	msg.length  = 8;
	hashCopy( (uint32_t *)msg.data );
	canSend( &msg );

	// TODO Check that all nodes confirmed their CRC and not just the number of nodes
	// TODO Retry upto 3 times if CRC has failed
	// Give all the nodes 1 second to respond
	timerSet( 1000 );
	uint8_t correct = 0;
	while( !timerPassed() ) {
		// Check if we have received a message, and
		// if that message is a CRC confirm message
		// and if that confirm is a confirm correct
		// message.
		if( canReceive(&msg) == MESSAGE_RECEIVED &&
				msg.id       == 0x107 &&
				msg.data[4]  == 0x3 ) {
			++correct;
		}
	}

	return correct == list->numNodes;
}

/**
 * Initialize the protocol for the programmer.
 */
void initProtocol( void ) {
	initCan();
	initTimer();
}

/**
 * Discover what nodes are available in the network.
 *
 * @param[out] The list of discovered nodes.
 */
void protocolDiscover( nodelist *list ) {
	// Try to get the other nodes in the network in
	// bootloader mode by spamming 0x100 messages for
	// 2 seconds.
	timerSet( 2000 );
	msg.id     = 0x100;
	msg.length = 0;
	while( !timerPassed() ) {
		canSend( &msg );
	}

	// Clear all recieved message untill now
	while( canReceive(&msg) == MESSAGE_RECEIVED );

	// Ask all the nodes in the network kindly
	// to identify themselves
	msg.id     = 0x101;
	msg.length = 0;
	canSend( &msg );

	// Wait 1 second to give every node the
	// change to register at the programmer
	timerSet( 200 );
	while( !timerPassed() ) {
		if( canReceive( &msg ) == MESSAGE_RECEIVED && msg.id == 0x102 ) {
			// Get the ID of the node that is registering
			uint32_t id = (msg.data[0]<<24) |
			              (msg.data[1]<<16) |
			              (msg.data[2]<<8 ) |
			              (msg.data[3]<<0 );

			// And save the ID
			list->ids[list->numNodes] = id;
			++list->numNodes;

			// Detect an overflow
			if( list->numNodes >= 512 )
				while(1);
		}
	}
}

/**
 * Programs the nodes in the network with a block.
 *
 * @param[in] list The list of nodes to program.
 * @param[in] start The start of the block to be flashed
 * @param[in] end The end of the block to be flashed
 * @param[in] sector The sector for the block to be placed in
 */
void protocolProgram( nodelist *list, uint8_t *start, uint8_t *end, uint8_t sector ) {

	// Programming 0 nodes is really fast!
	if( list->numNodes == 0 )
		return;

	// Select nodes to be programmed if not yet selected
	if ( !selected )
		selectNodes( list );

	// Make a datablock
	block.sector = sector;

	uint16_t i;
	uint8_t *index = start;
	for( i=0; i<4096; i++ ) {
		if( index < end ) {
			block.data[i] = *index;
			index++;
		} else {
			block.data[i] = 0x00;
		}
	}

	// Write a dataBlock to the selected nodes
	writeBlock( list, &block );

}

/**
 * Reboot the network.
 */
void protocolReset( void ) {
	msg.id     = 0x108;
	msg.length = 0;
	canSend( &msg );
}

/**
 * Select all nodes that are going to be reprogrammed.
 * @param[in] list The list of nodes to program.
 */
static void selectNodes( nodelist *list ) {

	msg.id     = 0x103;
	msg.length = 4;
	{
		uint16_t i;
		for( i=0; i<list->numNodes; ++i ) {
			msg.data[0] = ( (list->ids[i]&(0xFF<<24))>>24 );
			msg.data[1] = ( (list->ids[i]&(0xFF<<16))>>16 );
			msg.data[2] = ( (list->ids[i]&(0xFF<<8 ))>>8  );
			msg.data[3] = ( (list->ids[i]&(0xFF<<0 ))>>0  );

			canSend( &msg );
		}
	}
}
