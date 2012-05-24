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
 * The driver functions for the CAN peripheral.
 *
 * @author Chiel de Roest <M.A.deRoest@student.tudelft.nl> and Harmjan Treep <harmjan.treep@gmail.com>
 */

#include "LPC17xx.h"
#include "can.h"

/**
 * Setup the CAN peripheral.
 *
 * The pins for the 2th CAN peripheral are set to the right
 * mode, the power and clock are setup for the CAN peripheral
 * and CAN peripheral 2 is cleared and setup (timing and such).
 */
void initCan( void ) {
	// Set the processor pins to the correct mode
	LPC_PINCON->PINSEL0 &= ~(0x2<<8);  // Reset the function of Pin 0.4
	LPC_PINCON->PINSEL0 |= (0x2<<8);   // Set the function of Pin 0.4 to RD2
	LPC_PINCON->PINSEL0 &= ~(0x2<<10); // Reset the function of Pin 0.5
	LPC_PINCON->PINSEL0 |= (0x2<<10);  // Set the function of Pin 0.5 to TD2

	// Setup the power and clock for the peripheral
	LPC_SC->PCONP    |= (1<<14);  // Set the power bit for CAN peripheral 2
	LPC_SC->PCLKSEL0 &= ~(3<<28); // Clear the 2 CAN clock divider bits for can peripheral 2
	LPC_SC->PCLKSEL0 |= (1<<28);  // Set the clock divider for CAN peripheral 2 to 1,
	                              // so the main clock is the CAN clock
	LPC_SC->PCLKSEL0 &= ~(3<<30); // Clear the 2 clock divider bits for the can acceptance filter
	LPC_SC->PCLKSEL0 |= (1<<30);  // Set the clock divider for acceptance filter to 1

	// Setup peripheral related settings
	LPC_CAN2->MOD = 0x1; // Set the CAN peripheral in reset mode so you can change values
	LPC_CAN2->IER = 0x0; // Turn off all CAN related interrupts, they would not fire anyway
	LPC_CAN2->GSR = 0x0; // Set the error counters to 0
	// Clear everything you can via the command register
	LPC_CAN2->CMR = (0x1<<1) | // Abort transmission bit
	                (0x1<<2) | // Release receive buffer
	                (0x1<<3);  // Clear data overrun bit

	LPC_CAN2->BTR = 0xDCC001; // Set the bit rate of the CAN peripheral to 100kbit/s
	                          // BRP   = 1
	                          // SJW   = 3
	                          // TESG1 = 12
	                          // TESG2 = 5
	                          // SAM   = 1

	LPC_CAN2->MOD = 0; // Enable the CAN peripheral again

	LPC_CANAF->AFMR |= (1<<1); // Set the acceptance filter in bypass mode
}

/**
 * Receive a message over the CAN peripheral.
 *
 * @param[out] msg The message object to load the
 *                 received data into.
 * @return If a message was received MESSAGE_RECEIVED,
 *         otherwise NO_MESSAGE_RECEIVED.
 */
CanReceiveStatus canReceive( CanMessage *msg ) {
	if( (LPC_CAN2->SR & 1) == 0 ) // If we have not received a message return
		return NO_MESSAGE_RECEIVED;

	// Copy the message out of the register to the *msg object
	msg->length  = (LPC_CAN2->RFS & (15<<16))>>16; // Get the length of the message
	msg->id      = LPC_CAN2->RID & 4095;           // Get the ID of the message
	uint32_t tmp = LPC_CAN2->RDA;
	msg->data[0] = ( (tmp&(0xFF<<24))>>24 );       // Get the data sent in the message
	msg->data[1] = ( (tmp&(0xFF<<16))>>16 );
	msg->data[2] = ( (tmp&(0xFF<<8 ))>>8  );
	msg->data[3] = ( (tmp&(0xFF<<0 ))>>0  );
	tmp = LPC_CAN2->RDB;
	msg->data[4] = ( (tmp&(0xFF<<24))>>24 );
	msg->data[5] = ( (tmp&(0xFF<<16))>>16 );
	msg->data[6] = ( (tmp&(0xFF<<8 ))>>8  );
	msg->data[7] = ( (tmp&(0xFF<<0 ))>>0  );

	LPC_CAN2->CMR = (1<<2); // Release the receive buffer

	return MESSAGE_RECEIVED;
}

/**
 * Send a message over the CAN peripheral and busy
 * wait until we are sure the message was sent.
 *
 * Only buffer 1 is used in this implementation.
 */
void canSend( CanMessage *msg ) {
	// Copy the message date from the message to the registers
	LPC_CAN2->TFI1 &= ~(0xF<<16);          // Clear the length of the message to send
	LPC_CAN2->TFI1 |= (msg->length<<16);   // Set the length of the message to send
	LPC_CAN2->TID1  = (msg->id<<0);        // Set the ID of the message to be transmitted
	LPC_CAN2->TDA1  = (msg->data[3]<<24) | // Set the data of the message to be transmitted
	                  (msg->data[2]<<16) |
	                  (msg->data[1]<<8 ) |
	                  (msg->data[0]<<0 );
	LPC_CAN2->TDB1  = (msg->data[7]<<24) |
	                  (msg->data[6]<<16) |
	                  (msg->data[5]<<8 ) |
	                  (msg->data[4]<<0 );

	// Request for the processor to send the message
	LPC_CAN2->CMR = (1<<0) | // This is a transmission request
	                (1<<5);  // Use transmit buffer 1

	// Wait for the message to be transmitted
	while( (LPC_CAN2->SR & (1<<5)) == 0);
}
