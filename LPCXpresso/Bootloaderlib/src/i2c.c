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
#include "timer.h"
#include "LPC17xx.h"

/** Power Control for Peripherals bit mask */
#define CLKPWR_PCONP_BITMASK	0xEFEFF7DE

/** The I2C1 interface power/clock control bit */
#define	 CLKPWR_PCONP_PCI2C1  	(( uint32_t )( 1<<19 ))

/*******************************************************************//**
 * I2C Control register description
 *********************************************************************/
/** Assert acknowledge Clear bit */
#define I2C_CON_AAC				(( 1 << 2 ))
/** I2C interrupt Clear bit */
#define I2C_CON_SIC				(( 1 << 3 ))
/** STOP flag Clear bit  */
#define I2C_CON_STOC			(( 1 << 4 ))
/** START flag Clear bit */
#define I2C_CON_STAC			(( 1 << 5 ))
/** I2C interface Disable bit */
#define I2C_CON_ENC				(( 1 << 6 ))

#define I2CSTATE_IDLE     	0x000
#define I2CSTATE_PENDING  	0x001
#define I2CSTATE_SENT      0x002
#define I2CSTATE_ACK		0x100
#define I2CSTATE_NACK		0x101
#define I2CSTATE_ARB_LOSS  0x102

#define I2C_MES_START      0x08
#define I2C_MES_RSTART     0x10
#define I2C_MES_SLAW_ACK	0x18
#define I2C_MES_SLAW_NACK	0x20
#define I2C_MES_DATW_ACK	0x28
#define I2C_MES_ARBLOST		0x38
#define I2C_MES_SLAR_ACK   0x40
#define I2C_MES_SLAR_NACK  0x48
#define I2C_MES_DATR_ACK   0x50
#define I2C_MES_DATR_NACK  0x58

#define I2C_READ			0x0
#define I2C_WRITE			0x1

#define I2C_RD_BIT			0x01

#define MASTER_BUFSIZE 		30
#define SLAVE_BUFSIZE		30

#define I2C_MAX_TIMEOUT		0x00ffffff

static uint8_t i2cEngine( uint8_t RdWr );
static uint8_t i2cPoll( void );
static uint8_t i2cStart( void );
static void i2cStop( void );

uint32_t I2CMasterState = I2CSTATE_IDLE;
uint8_t  I2CMasterBuffer[MASTER_BUFSIZE];
uint8_t  I2CSlaveBuffer[SLAVE_BUFSIZE];
uint8_t  wrIndex  = 0;
uint8_t  rdIndex  = 0;
uint8_t  wrLength = 0;
uint8_t  rdLength = 0;

/**
 * Send 1 byte of data to the given device to be placed on the given address.
 * @param[in] device  The address of the device to send to.
 * @param[in] address The address for the data to be put.
 * @param[in] byte    The data to be send.
 */
uint8_t i2cSend( uint8_t device, uint16_t address, uint8_t byte ) {

	/* Data to be send */
	I2CMasterBuffer[0] = device;                   /* Address of slave device */
	I2CMasterBuffer[1] = ( address >> 8 ) & 0xff;  /* MSB of address of eeprom */
	I2CMasterBuffer[2] = address & 0xff;           /* LSB .. */
	I2CMasterBuffer[3] = byte;					   /* Data */

	uint8_t state = i2cEngine( I2C_WRITE );

  	/**
  	 * TODO: error handling when reading fails
  	 */

	timerDelay(5);  /* Delay for internal write cycle */

	return state;

}

/**
 * Receives a byte of data from the given address from the given device.
 * @param[in] device  The address of the device to receive from.
 * @param[in] address The address to receive from in the device.
 * @return            The byte of data in the device at the given address.
 */
uint8_t receiveI2C( uint8_t device, uint16_t address ) {

	I2CMasterBuffer[0] = device;					/* Address of slave device */
	I2CMasterBuffer[1] = ( address >> 8 ) & 0xff;   /* MSB of address of eeprom */
  	I2CMasterBuffer[2] = address & 0xff;     		/* LSB .. */
  	I2CMasterBuffer[3] = device | I2C_RD_BIT;	    /* Enable read flag */

  	uint8_t state = i2cEngine( I2C_READ );

  	/**
  	 * TODO: error handling when reading fails
  	 */

	return I2CSlaveBuffer[0];
}

/**
 * Start the I2C engine.
 * The engine listens the I2C bus and handles messages accordingly.
 * @param[in] RdWr Modus, either read (I2C_READ) or write (I2C_WRITE).
 */
static uint8_t i2cEngine( uint8_t RdWr ) {

	I2CMasterState = I2CSTATE_IDLE;
	wrIndex = 0;
	rdIndex = 0;

	wrLength = RdWr ? 4 : 3;
	rdLength = RdWr ? 0 : 1;

	if ( i2cStart() == 0 ) {
		i2cStop();
		return 0;
	}

	uint8_t result = i2cPoll();

	return result;

}

/**
 * Polls the I2C bus for new messages and handles them accordingly.
 */
static uint8_t i2cPoll( void ) {

	/* Poll as long as their has been no termination message. */
	while ( I2CMasterState < 0x100 ) {

		/* check interrupt bit for new messages */
		if ( ( LPC_I2C1->I2CONSET & I2C_CON_SIC ) == 0 ) {
			continue;
		}

		/* define type of message */
		uint8_t messageType = LPC_I2C1->I2STAT;

		switch ( messageType ) {

			case I2C_MES_START : // 0x08
				wrIndex = 0;
				LPC_I2C1->I2DAT = I2CMasterBuffer[wrIndex++];	    /* transmit device address */
				LPC_I2C1->I2CONCLR = ( I2C_CON_STAC | I2C_CON_STOC | I2C_CON_SIC );  /* reset interrupt and start condition bit */
				I2CMasterState = I2CSTATE_PENDING;
			break;

			case I2C_MES_RSTART : // 0x10
				rdIndex = 0;
				LPC_I2C1->I2DAT = I2CMasterBuffer[wrIndex++];
				LPC_I2C1->I2CONCLR = (I2C_CON_STAC | I2C_CON_SIC);
			break;

			case I2C_MES_SLAW_ACK : // 0x18
				LPC_I2C1->I2DAT = I2CMasterBuffer[wrIndex++];
				LPC_I2C1->I2CONCLR = ( I2C_CON_STAC | I2C_CON_STOC | I2C_CON_SIC );
			break;

			case I2C_MES_SLAW_NACK : // 0x20
				LPC_I2C1->I2CONSET = I2C_CON_STOC;
				LPC_I2C1->I2CONCLR = ( I2C_CON_STAC | I2C_CON_SIC );
				I2CMasterState = I2CSTATE_NACK;
			break;

			case I2C_MES_DATW_ACK : // 0x28

				/* send more data if not yet finished */
				if ( wrIndex < wrLength ) {
					LPC_I2C1->I2DAT = I2CMasterBuffer[wrIndex++];
					LPC_I2C1->I2CONCLR = ( I2C_CON_STAC | I2C_CON_STOC | I2C_CON_SIC );
				}
				else {
					if ( rdLength == 0 ) {
						/* send stop message to indicate writing has finished */
						LPC_I2C1->I2CONSET = I2C_CON_STOC;
						LPC_I2C1->I2CONCLR = ( I2C_CON_STAC | I2C_CON_SIC );
						I2CMasterState = I2CSTATE_ACK;
					}
					else {
						/* send repeated start message to initiate a read cycle */
						LPC_I2C1->I2CONSET = I2C_CON_STAC;
					}
				}
				LPC_I2C1->I2CONCLR = I2C_CON_SIC;
			break;

			case I2C_MES_ARBLOST : // 0x38
				LPC_I2C1->I2CONCLR = ( I2C_CON_STAC | I2C_CON_STAC | I2C_CON_SIC );
				I2CMasterState = I2CSTATE_ARB_LOSS;
			break;

			case I2C_MES_SLAR_ACK : // 0x40
				LPC_I2C1->I2CONCLR = I2C_CON_AAC;
				LPC_I2C1->I2CONCLR = ( I2C_CON_STAC | I2C_CON_STOC | I2C_CON_SIC );
			break;

			case I2C_MES_SLAR_NACK : // 0x48
				LPC_I2C1->I2CONSET = I2C_CON_STOC;
				LPC_I2C1->I2CONCLR = ( I2C_CON_STAC | I2C_CON_SIC );
				I2CMasterState = I2CSTATE_NACK;
			break;

			case I2C_MES_DATR_NACK : // 0x58
				I2CSlaveBuffer[rdIndex++] = LPC_I2C1->I2DAT;
				I2CMasterState = I2CSTATE_ACK;
				LPC_I2C1->I2CONSET = I2C_CON_STOC;
				LPC_I2C1->I2CONCLR = ( I2C_CON_STAC | I2C_CON_SIC );
			break;

			default :
				I2CMasterState = I2CSTATE_NACK;
				LPC_I2C1->I2CONCLR = I2C_CON_SIC;
			break;
		}

	}

	return I2CMasterState;

}

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
 	LPC_I2C1->I2CONCLR  = ( I2C_CON_AAC  | I2C_CON_STAC |
 							I2C_CON_ENC | I2C_CON_SIC ); /* Initilialize CONSET register, see table 381 */

 	LPC_I2C1->I2SCLH = 16; /* Select data rate  */
 	LPC_I2C2->I2SCLL = 16; /* Select duty cycle */

 	LPC_I2C1->I2ADR0 = deviceAddress; /* Register the slave device */

 	NVIC_DisableIRQ(I2C1_IRQn); /* Disable interrupts for I2C1 */

 	LPC_I2C1->I2CONSET = I2C_CON_ENC; /* Enable I2C1 peripheral */

}

/**
 * Deinitialize the I2C bus.
 */
void deinitI2C( void ) {
	i2cStop();
	LPC_I2C1->I2CONCLR = I2C_CON_ENC;
	LPC_SC->PCONP &= ~( CLKPWR_PCONP_PCI2C1 | CLKPWR_PCONP_BITMASK ); /* Power down the I2C peripheral. */
}

/**
 * Start the I2C1 peripheral.
 * Enter the master transmitter mode by setting the STA bit.
 * @return Boolean indicating whether the setting of the STA bit resulted in the I2C1 peripheral to become the master transmitter.
 */
static uint8_t i2cStart( void ) {

	uint32_t timer = 0;

	LPC_I2C1->I2CONSET = I2C_CON_STAC; /* Enter master mode by setting START bit */

	/* Wait for start condition */
	while ( ( LPC_I2C1->I2STAT != I2C_MES_START ) && ( timer < I2C_MAX_TIMEOUT ) ) {
		++timer;
	}

	return timer < I2C_MAX_TIMEOUT;

}

/**
 * Stop the I2C1 peripheral.
 * Disables the I2C1 peripheral by clearing the enable bit and thus disabling it.
 */
static void i2cStop( void ) {

	I2CMasterState = I2CSTATE_IDLE;

}
