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
#include <locale.h>

uint32_t getFileSize(FILE *file);
void scanNetwork();
void programNodes();
void error( uint8_t *error );

static FILE *uart;
static FILE *application;
static uint8_t *userApplication;
static uint8_t verbose = 0;
static uint8_t scan = 0;
static uint8_t program = 0;

void scanNetwork() {
	
	// TODO: append newly detected nodes to list of nodes

	printf("Scanning network...\n");

	uint8_t command = 0x01;
	if (verbose) printf("Send scanning request to programmer.\n");
	fwrite( &command, sizeof(uint8_t), 1, uart );

	uint8_t data;
	fread( &data , sizeof(uint8_t), 1, uart);
	if (verbose) printf("Programmer succesfully received scanning request.\n");

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

void programNodes() {

	// TODO: supply list with IDs of nodes and whether they should be programmed or not
	
	uint32_t fileSize = getFileSize( application );
	setlocale(LC_NUMERIC, "en_US.utf-8"); // to format large numbers

	printf("Programming all nodes with file \"%s\" (%'d bytes)...\n", userApplication, fileSize);

	uint8_t command = 0x02;
	if (verbose) printf("Send programming request to programmer.\n");
	fwrite( &command, sizeof(uint8_t), 1, uart );

	uint8_t data;
	fread( &data, sizeof(uint8_t), 1, uart );
	if (verbose) printf("Programmer succesfully received programming request.\n");

	fwrite( &fileSize, sizeof(uint32_t), 1, uart );
	
	uint32_t blockNumber;
	uint8_t blocksNeeded = (fileSize / 4096) + 1;
	uint8_t block[4096];

	if (verbose) printf("Sending data in %d blocks of 4kB.\n", blocksNeeded);
	command = 0x03;
	int i;
	for ( i=0; i<blocksNeeded; i++ ) {
		uint16_t blockSize = ( (i+1) == blocksNeeded ) ? fileSize % 4096 : 4096;
		fread(  block, sizeof(uint8_t), blockSize, application ); // read data from application file
		if ( ferror( application ) ) error("loading of application file failed");
		if (verbose) printf("Loading block of %'d bytes from application file succesfull.\n", blockSize);

		printf("Sending block #%d (%'d bytes)\n", i, blockSize);
		fwrite( block, sizeof(uint8_t), blockSize, uart ); // send data to programmer
		if (verbose) printf("Sending block to programmer succesfull.\n", blockSize);		

		if (verbose) printf("Sending mark for end of block to programmer.\n");
		fwrite( &command, sizeof(uint8_t), 1, uart );

		fread( &data, sizeof(uint8_t), 1, uart );
		if ( !data ) error( "error in programmer while programming nodes");
		if (verbose) printf("Programming nodes with block #%d succesfull.\n", i);

		fread( &data, sizeof(uint8_t), 1, uart );
		if ( data != command ) error( "block transmission not synchronized" );
		if (verbose) printf("Programmer has succesfully received the block.\n");
	}

	if (verbose) printf("Sending end of file mark to programmer.\n");
	command = 0x04;
	fwrite( &command, sizeof(uint8_t), 1, uart );

	fread( &data, sizeof(uint8_t), 1, uart );
	printf("Programmer succesfully received %'d bytes.\n", fileSize);

}

void error( uint8_t *errorString ) {
	printf("-- Error: %s\n\n", errorString);
	exit(1);
}

int main( int argc, char **argv ) {

	// supply device in arguments
	// long arguments e.g. --scan
	// list of nodes to flash [Y/N]
	int opt;
	while (( opt = getopt(argc, argv, "svp:")) > 0 )
	switch (opt) {
	case '?': 
		puts("Bad argument");
		exit(1);
		break;
	case 'p':
		program=1;
		userApplication = malloc( strlen( optarg ) );
		strcpy( userApplication, optarg );
		break;
	case 's':
		scan=1;
		break;
	case 'v':
		verbose=1;
		break;
	}

	if ( scan ) {
		uart = fopen( "/dev/ttyUSB0", "a+b" );
		scanNetwork();
		fclose( uart );
	}
	else if( program ) {
		//uart = fopen( "/dev/ttyUSB0", "a+b" );
		if ( !( uart=fopen( "/dev/ttyUSB0", "a+b" ) ) ) error( "failed to open /dev/ttyUSB0" );
		if ( !( application=fopen( userApplication, "rb" ) ) ) error( "failed to open binary file" );
		programNodes();
		fclose( application );
		fclose( uart );
	}
	else {
		printf("Bad input.\n");
	}

	free( userApplication );

	return 0;
}

uint32_t getFileSize(FILE *file) {
	fseek( file, 0L, SEEK_END);
	uint32_t size = ftell( application );
	fseek( file, 0L, SEEK_SET);
	return size;
}
