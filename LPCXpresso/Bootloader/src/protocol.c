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

#include "protocol.h"
#include "can.h"
#include "iap.h"
#include "hash.h"

/** The sector to program and the data we have received so far */
static DataBlock *block;
/** The index in the data for the point until which we received data */
static uint8_t *index;

/** If this node has been selected by the programmer for reprogramming */
static uint8_t selected = 0;

/** The message object used as temporary object */
static CanMessage msg;

/** The serial to use in the CAN protocol of this device */
static uint8_t serial[4];

/**
 * Set the serial of 4 bytes in a uint8_t array.
 *
 * The random ID is generated from the unique device ID
 * set by NXP during production.
 * @param array The array in which to set the random ID.
 */
static void setSerial( uint8_t *array ) {
	uint8_t i;
	for( i=0; i<4; i++ ) {
		array[i] = serial[i]; // TODO Confirm randomness of these 4 bytes
	}
}

/**
 * Send to the programmer the result of the CRC and the flashing of a block of data.
 * @param crcSuccess 0 if the CRC was wrong and 1 of the CRC was correct.
 * @param flashSuccess 0 if there was a problem while flashing the node and 1 if it went correctly.
 */
static void sendDataResult( uint8_t crcSuccess, uint8_t flashSuccess ) {
	msg.id      = 0x107;
	msg.length  = 5;
	setSerial( msg.data );

	msg.data[4] = (crcSuccess<<0) |  // CRC correct
				  (flashSuccess<<1); // Flash correct

	canSend( &msg );
}

/**
 * Initialize the CAN peripheral and setup everything needed to
 * comply to the CAN protocol.
 * @param[out] *blockIn The block to load the data in when receiving
 */
void initProtocol( DataBlock *blockIn ) {
	// Initialize the needed peripherals
	initCan();

	// Save the block pointer to communicate
	// received data back to main
	block  = blockIn;

	// Initialize the serial with 4 bytes of
	// the device serial
	uint8_t tmpSerial[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	getDeviceSerial( tmpSerial );
	{
		uint8_t i;
		for( i=0; i<4; i++ ) {
			serial[i] = tmpSerial[i];
		}
	}
}

/**
 * Detinitialize the CAN peripheral.
 */
void deinitProtocol( void ) {
	deinitCan();
}

/**
 * Check the state of the protocol and respond if necessary.
 * @return The action the bootloader needs to perform.
 */
ProtocolState check( void ) {
	if( canReceive( &msg ) == NO_MESSAGE_RECEIVED )
		return NO_ACTION;

	switch( msg.id ) {
	case 0x100: // Go into bootloading mode
		return BOOTLOADER;

	case 0x101: // Register at the programmer
		msg.id      = 0x102;
		msg.length  = 4;
		setSerial( msg.data );

		canSend( &msg );
		return NO_ACTION; // The bootloader should take no further action

	case 0x103: // Select this node for programming
		// Check if this node is selected for programming
		if( !selected ) {
			selected = 1;
			uint8_t i;
			for( i=0; i<4; ++i ) {
				if( msg.data[i] != serial[i] )
					selected = 0;
			}
		}

		return NO_ACTION; // The bootloader should take no further action

	case 0x104: // Address of data to come
		block->sector = msg.data[0];
		index = (block->data);

		// Initialize new hashes, create new hashes for every 4kB
		initHash();

		return NO_ACTION; // The bootloader should take no further action

	case 0x105: // New data
		// If this node is not selected to receive data ignore the CAN message
		if( !selected )
			return NO_ACTION;

		// If the index is further then the edge of the data region
		// this node must have missed a CRC message or something. Go
		// into error mode.
		if( index > &(block->data[4096]) ) {
			while(1); // TODO Better error
		}

		{
			uint8_t i;
			for( i=0; i<8; i++ ) {
				*index = msg.data[i];
				++index;
			}

			updateHash(&msg.data[0]);
			//for( i=0; i<4; i++ ) { TODO: remove
			//	hash[i] += (hashIndex+1) * (((msg.data[2*i+1]) << 8) | msg.data[2*i]);
			//}
			//++hashIndex;

		}
		return NO_ACTION; // The bootloader should take no further action

	case 0x106: // CRC of the received data
		// If this node is not selected to receive data ignore the CAN message
		if( !selected )
			return NO_ACTION;

		// Check if we have received enough messages
		if( index != &(block->data[4096]) ) {
			sendDataResult(0,0);
			return NO_ACTION;
		}

		combineHashes();
		if ( hashCheck(&msg.data[0]) ){
			return DATA_READY;
		}
		else {
			sendDataResult(0,0);
			return NO_ACTION;
		}

	case 0x108: // Reset the node
		return RESET_NODE;

	default: // If we do not know the ID do not do anything with it
		return NO_ACTION;
	}
}

/**
 * Return the status of the flashing of the node back to the protocol.
 * @param state The return state of the flashing of the node.
 */
void dataStatus( flashStatus state ) {
	switch( state ) {
	case FLASH_SUCCESS:
		sendDataResult(1,1);
		break;

	case COMPARE_FAILURE: // TODO More bits to give the programmer a better error.
	case BOOTLOADER_SECTOR:
	case INVALID_POINTER:
		sendDataResult(1,0);
		break;
	}
}
