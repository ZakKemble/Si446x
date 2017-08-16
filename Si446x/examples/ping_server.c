/*
 * Project: Si4463 Radio Library for AVR and Arduino (Ping server example)
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2017 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/si4463-radio-library-avr-arduino/
 */

/*
 * Ping server
 *
 * Listen for packets and send them back
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

#define CHANNEL 20
#define MAX_PACKET_SIZE 10

#define PACKET_NONE		0
#define PACKET_OK		1
#define PACKET_INVALID	2

typedef struct{
	uint8_t ready;
	int16_t rssi;
	uint8_t length;
	uint8_t buffer[MAX_PACKET_SIZE];
} pingInfo_t;

static int put(char c, FILE* stream)
{
	if(c == '\n')
		put('\r', stream);
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;
	return 0;
}

static FILE uart_io = FDEV_SETUP_STREAM(put, NULL, _FDEV_SETUP_WRITE);
static volatile pingInfo_t pingInfo;

void SI446X_CB_RXCOMPLETE(uint8_t length, int16_t rssi)
{
	// Make sure packet data will fit our buffer
	if(length > MAX_PACKET_SIZE)
		length = MAX_PACKET_SIZE;

	pingInfo.ready = PACKET_OK;
	pingInfo.rssi = rssi;
	pingInfo.length = length;

	Si446x_read((uint8_t*)pingInfo.buffer, length);

	// Radio will now be in idle mode
}

void SI446X_CB_RXINVALID(int16_t rssi)
{
	pingInfo.ready = PACKET_INVALID;
	pingInfo.rssi = rssi;

	// Radio will now be in idle mode
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
	Si446x_setTxPower(SI446X_MAX_TX_POWER);

	// Interrupts on
	sei();

	// Put into receive mode
	Si446x_RX(CHANNEL);
	
	uint32_t pings = 0;
	uint32_t invalids = 0;

	while(1)
	{
		puts_P(PSTR("Waiting for ping..."));

		// Wait for data
		while(pingInfo.ready == PACKET_NONE);
		
		if(pingInfo.ready != PACKET_OK)
		{
			// Got a corrupted packet
			invalids++;
			pingInfo.ready = PACKET_NONE;
			printf_P(PSTR("Invalid packet! Signal: %ddBm\n"), pingInfo.rssi);
			Si446x_RX(CHANNEL);
		}
		else
		{
			pings++;
			pingInfo.ready = PACKET_NONE;

			puts_P(PSTR("Got ping, sending reply..."));

			// Send back the data, once the transmission has completed go into receive mode
			Si446x_TX((uint8_t*)pingInfo.buffer, pingInfo.length, CHANNEL, SI446X_STATE_RX);

			puts_P(PSTR("Reply sent"));

			// Toggle LED
			PORTC ^= _BV(PORTC5);

			// Print out ping contents
			printf_P(PSTR("Signal strength: %ddBm\n"), pingInfo.rssi);
			printf_P(PSTR("Data from client: "));
			for(uint8_t i=0;i<sizeof(pingInfo.buffer);i++)
				printf_P(PSTR("%c"), pingInfo.buffer[i]);
			puts_P(PSTR(""));
		}
		
		printf_P(PSTR("Totals: %lu Pings, %lu Invalid\n------\n"), pings, invalids);
	}
}
