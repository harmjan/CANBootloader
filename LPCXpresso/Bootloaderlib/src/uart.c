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
 * The driver functions for the UART peripheral.
 *
 * @author Chiel de Roest <M.A.deRoest@student.tudelft.nl> and Harmjan Treep <harmjan.treep@gmail.com>
 */

#include "LPC17xx.h"
#include "uart.h"

static uint32_t baudrate = 9600;
volatile static uint8_t txBufferEmpty = 1;
volatile static uint8_t rxBuffer[4096];
volatile static uint16_t rxBufferCount = 0;

void uartSend( uint8_t *data, uint32_t length ) {

	while ( length-- > 0 ) {
		while ( !txBufferEmpty );  // Wait until transmission buffer is empty
		LPC_UART0->THR = *data++;
	}
}

uint8_t* uartReceive( uint32_t length ) {

	rxBufferCount = 0;
	while( rxBufferCount != length ) {}

	// TODO: Hmm.
	NVIC_DisableIRQ(UART0_IRQn);
	uint8_t data[length];
	uint32_t i = 0;
	while( i < length ) {
		data[i] = rxBuffer[i];
		++i;
	}
	NVIC_EnableIRQ(UART0_IRQn);

	return data;
}

void UART0_IRQHandler(void) {

	uint8_t interruptID = (LPC_UART0->IIR >> 1) & 0x07;
	switch ( interruptID ) {

	case 0x01: // Transmit Holding Register Empty (THRE)
		txBufferEmpty = LPC_UART0->LSR >> 5; // Check if there is more data to be send
		break;
	case 0x02: // Receive Data Available (RDA)
		rxBuffer[rxBufferCount++] = LPC_UART0->RBR;
		rxBufferCount = ( rxBufferCount == 4096 ) ? 0 : rxBufferCount;
		break;
	case 0x03: // Receive Line Status (RLS)

		if ( LPC_UART0->LSR & 0x9E ) { // Check for errors OE, PE, FE, BI, RXFE
			LPC_UART0->RBR; // Clear interrupt
			return;
		}
		if ( LPC_UART0->LSR & 0x01 ) { // Check for Receiver Data Ready (RDR)
			rxBuffer[rxBufferCount++] = LPC_UART0->RBR;
			rxBufferCount = ( rxBufferCount == 4096 ) ? 0 : rxBufferCount;
		}

		break;
	case 0x06: // Character Time-out Indicator (CTI)
		LPC_UART0->RBR; // Clear interrupt
		break;
	default:  // Ignore reserved combinations
		break;
	}
}

void initUART( void ) {

		uint8_t  clk_div, clk_div_frac;
		uint32_t clk;

		LPC_SC->PCONP |= (1 << 3);	// Set the power bit for UART0
		LPC_PINCON->PINSEL0 &= ~0x000000F0; // Reset P0.2 and P0.3 pins
		LPC_PINCON->PINSEL0 |=  0x00000050; // Assign TXD0 function to P0.2 and RXD0 function to P0.3

		clk_div = (LPC_SC->PCLKSEL0 >> 6) & 0x03; // Clock divider for UART0
		switch (clk_div) {
		default:
		case 0x00:
			clk = SystemCoreClock / 4;
			break;
		case 0x01:
			clk = SystemCoreClock;
			break;
		case 0x02:
			clk = SystemCoreClock / 2;
			break;
		case 0x03:
			clk = SystemCoreClock / 8;
		}

		LPC_UART0->LCR = 0x83; // Set DLAB bit, 8-bit words, 1 stop bit, no parity
		clk_div_frac   = (clk / 16) / baudrate; // Fractional divider
		LPC_UART0->DLM = clk_div_frac / 256;
		LPC_UART0->DLL = clk_div_frac % 256;
		LPC_UART0->LCR = 0x03; // Clear DLAB
		LPC_UART0->FCR = 0x07; // Enable FIFO

		NVIC_EnableIRQ(UART0_IRQn); // Enable UART0 interrupts
		LPC_UART0->IER |= 0x07;     // Enable RBR, THRE, RX Line Status interrupts
}

void deinitUART( void ) {
	// TODO: implement
}
