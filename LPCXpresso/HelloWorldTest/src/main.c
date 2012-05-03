/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
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

#define DEBUG

#include "lpc17xx_can.h"
#include "lpc17xx_pinsel.h"
#include "debug_frmwrk.h"

CAN_MSG_Type msg;

PINSEL_CFG_Type Port0Pin4;
PINSEL_CFG_Type Port0Pin5;

int main(void) {

	goto serialTest;

	SystemCoreClockUpdate();

	debug_frmwrk_init();

	_DBG("CAN TEST\n\n");

	Port0Pin4.Portnum   = PINSEL_PORT_0;
	Port0Pin4.Pinnum    = PINSEL_PIN_4;
	Port0Pin4.Funcnum   = PINSEL_FUNC_2;
	Port0Pin4.Pinmode   = PINSEL_PINMODE_PULLUP;
	Port0Pin4.OpenDrain = PINSEL_PINMODE_NORMAL;

	Port0Pin5.Portnum   = PINSEL_PORT_0;
	Port0Pin5.Pinnum    = PINSEL_PIN_5;
	Port0Pin5.Funcnum   = PINSEL_FUNC_2;
	Port0Pin5.Pinmode   = PINSEL_PINMODE_PULLUP;
	Port0Pin5.OpenDrain = PINSEL_PINMODE_NORMAL;

	PINSEL_ConfigPin(&Port0Pin4);
	PINSEL_ConfigPin(&Port0Pin5);

	CAN_Init(LPC_CAN2,125000);
	CAN_ModeConfig(LPC_CAN2,CAN_OPERATING_MODE,ENABLE);

	msg.id     = 0x100;
	msg.len    = 1;
	msg.dataA[0] = 0x08;
	msg.type   = DATA_FRAME;
	msg.format = STD_ID_FORMAT;

	_DBG("Starting transmissions\n\n");

	while(1) {
		if( CAN_SendMsg(LPC_CAN2,&msg) == ERROR )
			_DBG("\tSending error\n");
	}

serialTest:
	testserialNumber();

	return 0 ;
}
