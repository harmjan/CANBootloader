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
 * The functions for the host to communicate with the programmer.
 *
 * @author Chiel de Roest <M.A.deRoest@student.tudelft.nl> and Harmjan Treep <harmjan.treep@gmail.com>
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

uint32_t getFileSize(FILE *file);
void scanNetwork();
void programNodes();

static FILE *uart;
static FILE *application;
static uint8_t verbose = 0;
static uint8_t scan = 0;
static uint8_t program = 0;

void scanNetwork(){
	
	printf("Scanning network...\n");

	uint8_t command = 0x01;
	if (verbose) printf("Send request to programmer.\n");
	fwrite( &command, sizeof(uint8_t), 1, uart );

	uint8_t data;
	fread( &data , sizeof(uint8_t), 1, uart);
	if (verbose) printf("Programmer succesfully received request.\n");

	uint16_t numNodes;
	if (verbose) printf("Querying programmer for amount of responding nodes.\n");
	fread( &numNodes, sizeof(uint16_t), 1, uart);
	printf( "Found %d nodes active.\n", numNodes );

	uint32_t nodeList[numNodes];
	uint16_t i;
	for( i=0; i<numNodes; i++ ){
		fread( nodeList+i, sizeof(uint32_t), 1, uart );
		printf( "#%d: 0x%08x\n", i, nodeList[i] );
	}

	if (verbose) printf("Send success message to the programmer.\n");
	fwrite( &command, sizeof(uint8_t), 1, uart );

	return;
}

int main( int argc, char **argv ) {

	// supply device in arguments
	// binary
	// --scan
	// list of nodes to flash [Y/N]
	int opt;
	while (( opt = getopt(argc, argv, "sv")) > 0)  
	switch (opt) {
	case '?': puts("Bad argument"); break;
	case 's': 
		scan=1;
		break;
	case 'v': 
		verbose=1;
		break;
        default: break;
	}
	if ( scan ) {
		uart = fopen( "/dev/ttyUSB0", "a+b" );
		scanNetwork();
		fclose( uart );
	}
	else if( program ) {

	}
	else {
		printf("Bad input. Nothing to do.\n");
	}

	return 0;
}

uint32_t getFileSize(FILE *file) {
	fseek( file, 0L, SEEK_END);
	uint32_t size = ftell( application );
	fseek( file, 0L, SEEK_SET);
	return size;
}
