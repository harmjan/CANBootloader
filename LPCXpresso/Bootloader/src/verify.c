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
 * The implementation of functions to verify data, e.g. by CRC, hashing.
 *
 * @author Chiel de Roest <M.A.deRoest@student.tudelft.nl> and Harmjan Treep <harmjan.treep@gmail.com>
 */

#include "verify.h"
#include "crc.h"

/**
 * Generates a signature for the given data.
 * @param[in] start      A pointer to the start of the data.
 * @param[in] end        A pointer to the end of the data.
 * @param[out] signature A pointer to the generated signature.
 */
void generateSignature( uint8_t *start, uint8_t *end, uint32_t *signature ) {

	*signature = generateCRC(start, end - start + 1);

}

/**
 * Verifies the given data with the supplied signature.
 * @param[in] start      A pointer to the start of the data.
 * @param[in] end        A pointer to the end of the data.
 * @param[out] signature A pointer to the supplied signature.
 */
uint8_t verifyData( uint8_t *start, uint8_t *end, uint32_t *signature ) {

	uint32_t genSig = generateCRC(start, end - start + 1);
	return genSig == *signature;

}

/**
 * Intializes the signature generation function.
 */
void initVerify( void ) {

	initCRC();

}
