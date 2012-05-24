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

#include <stdint.h>

#ifndef I2C_H__
#define I2C_H__

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

void initI2C( uint32_t deviceAddress );
void deInitI2C( void );
uint8_t sendI2C( uint8_t device, uint16_t address, uint8_t byte );
uint8_t receiveI2C( uint8_t device, uint16_t address );
static uint8_t I2CEngine( uint8_t RdWr );
static uint8_t pollI2C( void );
static uint8_t startI2C( void );
static void stopI2C( void );

#endif
