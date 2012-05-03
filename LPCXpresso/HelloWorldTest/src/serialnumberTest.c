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

#include <stdio.h>
#define IAP_LOCATION 0x1FFF1FF1

// TODO: insert other definitions and declarations here

int testserialNumber(void) {

//	printf("Starting\n");

	unsigned long command[5];
	command[0] = 58;
	unsigned long result[5];
	result[0] = 0;
	result[1] = 0;
	result[2] = 0;
	result[3] = 0;
	result[4] = 0;

	typedef void (*IAP)(unsigned long [], unsigned long[]);
	IAP iap_entry;
	iap_entry = (IAP) IAP_LOCATION;
	iap_entry(command, result);

	return 0 ;
}
