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
 * The functions to create and check hashes.
 * It implements a fast iterative hash function to reduce hash generation time when all packets are received.
 *
 * @author Chiel de Roest <M.A.deRoest@student.tudelft.nl> and Harmjan Treep <harmjan.treep@gmail.com>
 */

#include "hash.h"

/** The iteratively generated hashes */
static uint32_t hash[HASH_COUNT];

/** The number of iterations that are already done */
static uint16_t hashIndex;

/**
 * Creates space for the hashes and sets the already computed hash values to zero.
 */
void initHash( void ) {

	uint8_t i;
	for ( i=0; i<HASH_COUNT; i++ ) {
		hash[i] = 0;
	}
	hashIndex = 0;

}

/**
 * Deinitializes the hash functions.
 */
void deinitHash( void ) {
}

/**
 * Iteratively updates the generated hashes with the newly supplied data.
 *
 * It uses an recursive arithmetic function to detect sequence errors.
 * 4 hashes will be generated, the 1st hash will cover the 1st two bytes
 * of the data, the 2nd hash will cover the 2nd two bytes, etc.
 *
 * If execution is at iteration i, the 1st hash is updated by the function
 * hash(i) = hash(i-1) + [ (i+1) * (data(0) OR data(1)) ]
 * where hash(i-1) was the previous hash, data(i) is the i'th byte in data
 *
 * @param[in] data A pointer to the data for the hash to be based on.
 */
void hashUpdate( uint8_t *data ) {

	uint8_t i;
	for ( i=0; i<HASH_COUNT; i++ ) {
		hash[i] += (hashIndex+1) * (( *(data + (2*i+1)) << 8 ) | *(data + 2*i));
	}
	++hashIndex;

}

/**
 * Combines the generated hashes to a smaller amount of hashes to be transmitted.
 *
 * The algorithm creates HASH_COUNT_FINAL amount of hashes from HASH_COUNT hashes.
 * It uses the exclusive OR operator on the i and (HASH_COUNT_FINAL - i) values
 * and stores it in hash(i), to be left with half the amount of hashes.
 */
void hashCombine( void ) {

	uint8_t i;
	for ( i=0; i<HASH_COUNT_FINAL; i++ ) {
		hash[i] ^= hash[HASH_COUNT-i];
	}

}

/**
 * Checks a given hash value with the generated hash and returns the result.
 *
 * @param[in] receivedHash A pointer to the received hash.
 * @return                 The result of the check: 0 if success, 1 if the hashes don't correspond.
 */
uint8_t hashCheck( uint32_t *receivedHash ) {

	uint8_t i;
	for ( i=0; i<HASH_COUNT_FINAL; i++ ) {
		if ( *receivedHash != hash[i] ) {
			return 1;
		}
		++receivedHash;
	}
	return 0;

}

/**
 * Copies the generated hashes into a specific place in memory.
 * @param[out] storage A pointer to the generated hashes.
 */
void hashCopy( uint32_t *storage ) {

	uint8_t i;
	for ( i=0; i<HASH_COUNT_FINAL; i++ ) {
		*storage = hash[i];
		++storage;
	}

}
