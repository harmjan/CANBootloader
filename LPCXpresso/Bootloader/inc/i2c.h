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
 * I2C Control Clear register description
 *********************************************************************/
/** Assert acknowledge Clear bit */
#define I2C_I2CONCLR_AAC			(( 1 << 2 ))
/** I2C interrupt Clear bit */
#define I2C_I2CONCLR_SIC			(( 1 << 3 ))
/** STOP flag Clear bit  */
#define I2C_I2CONCLR_STOC			(( 1 << 4 ))
/** START flag Clear bit */
#define I2C_I2CONCLR_STAC			(( 1 << 5 ))
/** I2C interface Disable bit */
#define I2C_I2CONCLR_I2ENC			(( 1 << 6 ))

#define I2CONSET_I2EN		0x00000040  /* I2C Control Set Register */
#define I2CONSET_AA			0x00000004
#define I2CONSET_SI			0x00000008
#define I2CONSET_STO		0x00000010
#define I2CONSET_STA		0x00000020

#define I2CONCLR_AAC		0x00000004  /* I2C Control clear Register */
#define I2CONCLR_SIC		0x00000008
#define I2CONCLR_STAC		0x00000020
#define I2CONCLR_I2ENC		0x00000040


void initI2C( uint32_t deviceAddress );
void sendI2C( uint8_t byte );
uint8_t receiveI2C();

#endif
