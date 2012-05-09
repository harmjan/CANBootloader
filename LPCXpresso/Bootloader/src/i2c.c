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
 * The driver functions for the I2C peripheral.
 *
 * @author Chiel de Roest <M.A.deRoest@student.tudelft.nl> and Harmjan Treep <harmjan.treep@gmail.com>
 */

#include "i2c.h"
#include "LPC17xx.h"

/**
 * Initializes the I2C1 peripheral.
 * The I2C1 peripheral is set as the master device and a device using the specified address is registered as a slave device.
 * @param[in] deviceAddress The address of the slave device on the I2C bus.
 */
void initI2C( uint32_t deviceAddress ) {

	// Configure I/O pins
	LPC_PINCON->PINSEL1     |=  ( 0x3 << 6 ); /* Enable SDA1 */
	LPC_PINCON->PINSEL1     |=  ( 0x3 << 8 ); /* Enable SCL1 */
	LPC_PINCON->PINMODE1    &= ~( 0x1 << 6 );
	LPC_PINCON->PINMODE1    |=  ( 0x1 << 7 );
	LPC_PINCON->PINMODE1    &= ~( 0x1 << 8 );
	LPC_PINCON->PINMODE1    |=  ( 0x1 << 9 );
	LPC_PINCON->PINMODE_OD0 |=  ( 0x3 << 19 );

	LPC_SC->PCLKSEL1   |= ( 0x3 << 6 ); /* Select PCLK_I2C1 clock */
 	LPC_SC->PCONP      |= CLKPWR_PCONP_PCI2C1 & CLKPWR_PCONP_BITMASK; /* Set up clock and power for I2C1 module */
 	LPC_I2C1->I2CONSET  = ( I2C_I2CONCLR_AAC   | I2C_I2CONCLR_STAC |
 							I2C_I2CONCLR_I2ENC | I2C_I2CONCLR_SIC ); /* Initilialize CONSET register, see table 381 */

 	LPC_I2C1->I2SCLH = 16; /* Select data rate  */
 	LPC_I2C2->I2SCLL = 16; /* Select duty cycle */

 	LPC_I2C1->I2ADR0 = deviceAddress; /* Register the slave device */

 	LPC_I2C1->I2CONSET = I2CONSET_I2EN; /* Enable I2C1 peripheral */

}

void deInit() {

}

void sendI2C( uint8_t byte ) {

}

uint8_t receiveI2C() {

	// poll for incoming messages

	// return value

	return 0;
}
