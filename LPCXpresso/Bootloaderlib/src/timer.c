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
 * The functions to interface with the processor timers.
 *
 * Timer0 is used by the timerSet and timerPassed functions and
 * Timer1 is used to generate a blocking delay.
 *
 * @author Chiel de Roest <M.A.deRoest@student.tudelft.nl> and Harmjan Treep <harmjan.treep@gmail.com>
 */

#include "LPC17xx.h"

/**
 * Initialize the timer drivers we are going to use
 */
void initTimer( void ) {
	LPC_SC->PCONP |= (1<<1); // Enable power to Timer0
	LPC_SC->PCONP |= (1<<2); // Enable power to Timer1

	// Setting 01 in bit 2:3 sets the clockdivider for Timer0
	// to 1, setting 01 in bit 4:5 does this for Timer1
	LPC_SC->PCLKSEL0 &= ~(3<<2); // Clear the clock divider for Timer0
	LPC_SC->PCLKSEL0 |=  (1<<2); // Set the clock divider for Timer0 to 1
	LPC_SC->PCLKSEL0 &= ~(3<<4); // Clear the clock divider for Timer1
	LPC_SC->PCLKSEL0 |=  (1<<4); // Set the clock divider for Timer1 to 1

	// Setup the match registers so that the timer stops
	// when it matches
	LPC_TIM0->MCR  = 0;      // Reset the Match control register
	LPC_TIM0->MCR |= (1<<2); // Stop the counter on match with MR0
	LPC_TIM1->MCR  = 0;      // Reset the Match control register
	LPC_TIM1->MCR |= (1<<2); // Stop the counter on match with MR0

	// Set the prescale counters
	LPC_TIM0->PC = 0;
	LPC_TIM1->PC = 0;
}

/**
 * Deinitialize the timer peripheral.
 */
void deinitTimer( void ) {
	LPC_SC->PCONP &= ~(1<<1); // Disable power to Timer0
	LPC_SC->PCONP &= ~(1<<2); // Disable power to Timer1
}

/**
 * Busy wait a certain amount of time.
 *
 * @param[in] milliSeconds The amount of milliSeconds to wait.
 */
void timerDelay( uint16_t milliSeconds ) {
	LPC_TIM1->TCR = (1<<1); // Reset the timer
	LPC_TIM1->MR0 = (SystemCoreClock/1000+1) * milliSeconds; // Set the match register
	LPC_TIM1->TCR = (1<<0); // Start Timer1

	// Wait for Timer1 to be reset
	while( LPC_TIM1->TCR & (1<<0) );
}

/**
 * Set the timer for an amount of microseconds.
 *
 * @param[in] milliSeconds The amount of milliSeconds to wait.
 */
void timerSet( uint32_t milliSeconds ) {
	LPC_TIM0->TCR = (1<<1); // Reset the timer

	LPC_TIM0->MR0 = (SystemCoreClock/1000+1) * milliSeconds; // Set the match register

	LPC_TIM0->TCR = (1<<0); // Start Timer0
}

/**
 * Return if the set timer has passed yet.
 *
 * @return If the timer has passed yet.
 */
uint8_t timerPassed( void ) {
	// If Timer0 has reset than the set time has passed
	return (LPC_TIM0->TCR & (1<<0)) == 0;
}
