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
 * The functions to communicate with the host.
 *
 * @author Chiel de Roest <M.A.deRoest@student.tudelft.nl> and Harmjan Treep <harmjan.treep@gmail.com>
 */

#include "LPC17xx.h"
#include "host.h"
#include "uart.h"

uint8_t hostListen() {
	uint8_t destination;
	hostReceiveData( &destination, 1 );
	return destination;
}

uint16_t hostListen16() {
	uint8_t num1 = hostListen();
	uint8_t num2 = hostListen();
	return num1 ^ (num2 << 8);
}

uint32_t hostListen32() {
	uint16_t num1 = hostListen16();
	uint16_t num2 = hostListen16();
	return num1 ^ (num2 << 16);
}

void hostSendResponse( uint8_t response ) {
	hostSendData( &response, 1 );
}

void hostSendData( uint8_t *data, uint32_t length ) {
	uartSend( data, length );
}

void hostReceiveData( uint8_t *destination, uint32_t length ) {
	uartReceive( destination, length );
}

void initHost() {
	initUART();
}

void deinitHost() {}
