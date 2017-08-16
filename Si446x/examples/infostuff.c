/*
 * Project: Si4463 Radio Library for AVR and Arduino (Dump and info example)
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2017 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/si4463-radio-library-avr-arduino/
 */

/*
 * Dump properties and show chip info
 */

#define BAUD 1000000

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <util/atomic.h>
#include <util/setbaud.h>
#include <util/delay.h>
#include <stdio.h>
#include "Si446x.h"

#define CHANNEL 10

static int put(char c, FILE* stream)
{
	if(c == '\n')
		put('\r', stream);
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;
	return 0;
}

static FILE uart_io = FDEV_SETUP_STREAM(put, NULL, _FDEV_SETUP_WRITE);

void SI446X_CB_RXCOMPLETE(uint8_t length, int16_t rssi)
{
	(void)(length); // Get rid of unused variable warnings
	(void)(rssi);

	Si446x_RX(CHANNEL);
}

void SI446X_CB_RXINVALID(int16_t rssi)
{
	(void)(rssi);

	Si446x_RX(CHANNEL);
}

static void dump(uint8_t group)
{
	puts_P(PSTR("Property group dump:"));
	
	uint8_t length = Si446x_dump(NULL, group); // Get group size by passing NULL as the output buffer
	uint8_t props[length]; // Allocate space for the properties
	Si446x_dump(props, group); // Get the properties

	// Print the properties
	printf_P(PSTR("Group: %02X\n"), group);
	for(uint8_t i=0;i<length;i++)
		printf_P(PSTR("%02X: %02X\n"), i, props[i]);

	puts_P(PSTR("Dump end"));
}

void main(void)
{
	clock_prescale_set(clock_div_1);
	
	// UART
	//PORTD |= _BV(PORTD0);
	//DDRD |= _BV(DDD1);
	UBRR0 = UBRR_VALUE;
#if USE_2X
	UCSR0A = _BV(U2X0);
#endif
	UCSR0B = _BV(TXEN0);

	// LED indicator
	DDRC |= _BV(DDC5);
	PORTC |= _BV(PORTC5);

	stdout = &uart_io;

	// Start up
	Si446x_init();

	// Interrupts on
	sei();

	while(1)
	{
		dump(SI446X_PROP_GROUP_GLOBAL);

		si446x_info_t info;
		Si446x_getInfo(&info);
		
		Si446x_sleep();

		printf_P(PSTR("Chip rev: %hhu\n"), info.chipRev);
		printf_P(PSTR("Part: %u\n"), info.part);
		printf_P(PSTR("Part build: %hhu\n"), info.partBuild);
		printf_P(PSTR("ID: %u\n"), info.id);
		printf_P(PSTR("Customer: %hhu\n"), info.customer);
		printf_P(PSTR("ROM ID: %hhu\n"), info.romId); // 03 = revB1B, 06 = revC2A
		_delay_ms(20);
		
		printf_P(PSTR("Rev ext: %hhu\n"), info.revExternal);
		printf_P(PSTR("Rev branch: %hhu\n"), info.revBranch);
		printf_P(PSTR("Rev int: %hhu\n"), info.revInternal);
		printf_P(PSTR("Patch: %u\n"), info.patch);
		printf_P(PSTR("Func: %hhu\n"), info.func);
		puts_P(PSTR("------"));

		_delay_ms(2000);
	}
}
