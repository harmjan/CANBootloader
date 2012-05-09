/**
 *
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include <NXP/crp.h>

// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP ;

#include <stdio.h>

#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_systick.h"

PINSEL_CFG_Type Port0Pin22;

uint8_t state = 0;
uint32_t counter = 0;
void SysTick_Handler(void) {
	counter++;
	if( counter < 50 )
		return;
	counter = 0;

	if( state ) {
		state = 0;
		GPIO_ClearValue( 0, 1<<22 );
	} else {
		state = 1;
		GPIO_SetValue( 0, 1<<22 );
	}
}

int main(void) {
	Port0Pin22.Portnum   = PINSEL_PORT_0;
	Port0Pin22.Pinnum    = PINSEL_PIN_22;
	Port0Pin22.Funcnum   = PINSEL_FUNC_0;
	Port0Pin22.Pinmode   = PINSEL_PINMODE_PULLUP;
	Port0Pin22.OpenDrain = PINSEL_PINMODE_NORMAL;

	PINSEL_ConfigPin( &Port0Pin22 );

	GPIO_SetDir( 0, 1<<22, 1 );

	SystemCoreClockUpdate();
	SYSTICK_InternalInit( 10 );
	SYSTICK_Cmd( ENABLE );
	SYSTICK_IntCmd( ENABLE );

	while(1);
	return 0 ;
}
