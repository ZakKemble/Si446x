/*
 * Project: Si4463 Radio Library for AVR and Arduino (Sleep example)
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2017 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/si4463-radio-library-avr-arduino/
 */

/*
 * This example shows how to safely enter sleep mode without missing any callbacks
 */

#define BAUD 1000000

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <util/atomic.h>
#include <util/setbaud.h>
#include <util/delay.h>
#include <stdio.h>
#include "Si446x.h"

#define CHANNEL 10

static volatile uint8_t rxlen;
static volatile uint8_t rxcomplete;
static volatile uint8_t rxinvalid;
static volatile uint8_t rxbegin;
static volatile uint8_t sent;
static volatile uint8_t wut;

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
	(void)(rssi);
	rxcomplete = 1;
	rxlen = length;

	// Dont enter RX mode yet, we still need to read the data. We'll stay in SI446X_IDLE_MODE
	//Si446x_RX(CHANNEL);
}

void SI446X_CB_RXINVALID(int16_t rssi)
{
	(void)(rssi);
	rxinvalid = 1;
	Si446x_RX(CHANNEL);
}

void SI446X_CB_RXBEGIN(int16_t rssi)
{
	(void)(rssi);
	rxbegin = 1;
}

void SI446X_CB_SENT(void)
{
	sent = 1;
}

void SI446X_CB_WUT(void)
{
	wut = 1;
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

	Si446x_setupCallback(SI446X_CBS_RXBEGIN | SI446X_CBS_SENT, 1); // Enable packet RX begin and packet sent callbacks
	Si446x_setupWUT(0, 16384, 0, SI446X_WUT_RUN); // Run WUT every 2 seconds
	
	Si446x_RX(CHANNEL);

	uint8_t testData[] = {
		2,
		3,
		4,
		5,
		6,
		7
	};

	while(1)
	{
		uint8_t localRxinvalid;
		uint8_t localRxcomplete;
		uint8_t localRxbegin;
		uint8_t localSent;
		uint8_t localWut;
		
		// Copy and clear flags while the Si446x interrupt is turned off so we don't miss any callbacks between checking the flag and clearing it
		SI446X_NO_INTERRUPT()
		{
			localRxinvalid = rxinvalid;
			localRxcomplete = rxcomplete;
			localRxbegin = rxbegin;
			localSent = sent;
			localWut = wut;

			rxcomplete = 0;
			rxinvalid = 0;
			rxbegin = 0;
			sent = 0;
			wut = 0;
		}

		if(localRxcomplete)
		{
			// Read data
			uint8_t data[rxlen];
			Si446x_read(data, rxlen);

			// Print value of first byte
			printf_P(PSTR("Got new packet: %hhu\n"), data[0]);
			
			// Back to receive mode
			Si446x_RX(CHANNEL);
		}

		if(localRxinvalid)
			puts_P(PSTR("Invalid packet"));

		if(localRxbegin)
			puts_P(PSTR("New packet start"));

		if(localSent)
			puts_P(PSTR("Packet sent"));

		if(localWut)
		{
			puts_P(PSTR("Sending packet"));
			Si446x_TX(testData, sizeof(testData), CHANNEL, SI446X_STATE_RX); // Transmit and go to receive mode once sent
			testData[0]++;
		}
		
		_delay_ms(50); // Artificial processing delay

		cli(); // Global interrupts off
		if(!wut && !sent && !rxbegin && !rxinvalid && !rxcomplete) // Make sure no callbacks were called during processing
		{
			puts_P(PSTR("Going to sleep"));

			sleep_enable();
			sleep_bod_disable();
			sei();
			sleep_cpu();
			sleep_disable();

			puts_P(PSTR("Wakeup"));
		}
		else
		{
			// A callback was ran while we were processing so don't go to sleep. Instead loop again to process the new stuff
			
			sei();
			puts_P(PSTR("Did not sleep!"));
		}
	}
}
