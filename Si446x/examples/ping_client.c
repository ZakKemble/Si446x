/*
 * Project: Si4463 Radio Library for AVR and Arduino (Ping client example)
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2017 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/si4463-radio-library-avr-arduino/
 */

/*
 * Ping client
 *
 * Time how long it takes to send some data and get a reply
 * Should be around 5-6ms with default settings
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
#define TIMEOUT 1000

#define PACKET_NONE		0
#define PACKET_OK		1
#define PACKET_INVALID	2

typedef struct{
	uint8_t ready;
	uint32_t timestamp;
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
static volatile uint32_t milliseconds;
static volatile pingInfo_t pingInfo;

void SI446X_CB_RXCOMPLETE(uint8_t length, int16_t rssi)
{
	// Make sure packet data will fit our buffer
	if(length > MAX_PACKET_SIZE)
		length = MAX_PACKET_SIZE;

	pingInfo.ready = PACKET_OK;
	pingInfo.timestamp = milliseconds;
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

uint32_t millis(void)
{
	uint32_t ms;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		ms = milliseconds;
	}
	return ms;
}

void main(void)
{
	clock_prescale_set(clock_div_1);

	// Timer 0 settings for approx. millisecond tracking
	TCCR0A = _BV(WGM01);
	TCCR0B = (_BV(CS01)|_BV(CS00));
	TIMSK0 = _BV(OCIE0A);
	OCR0A = ((F_CPU / 64) / 1000);

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

	uint8_t counter = 0;
	uint32_t sent = 0;
	uint32_t replies = 0;
	uint32_t timeouts = 0;
	uint32_t invalids = 0;

	while(1)
	{
		// Make data
		char data[MAX_PACKET_SIZE] = {0};
		sprintf_P(data, PSTR("test %hhu"), counter);
		counter++;

		pingInfo.ready = PACKET_NONE;

		printf_P(PSTR("Sending data: %s\n"), data);

		uint32_t startTime = millis();

		// Send the data
		Si446x_TX(data, sizeof(data), CHANNEL, SI446X_STATE_RX);
		sent++;
		
		puts_P(PSTR("Data sent, waiting for reply..."));

		uint8_t success;

		// Wait for reply with timeout
		uint32_t sendStartTime = millis();
		while(1)
		{
			success = pingInfo.ready;
			if(success != PACKET_NONE)
				break;
			else if(millis() - sendStartTime > TIMEOUT)
				break;
		}

		if(success == PACKET_NONE)
		{
			puts_P(PSTR("Ping timed out"));
			timeouts++;
		}
		else if(success == PACKET_INVALID)
		{
			// Got a corrupted packet
			printf_P(PSTR("Invalid packet! Signal: %ddBm\n"), pingInfo.rssi);
			invalids++;
		}
		else
		{
			// If success toggle LED and send ping time over UART
			uint16_t totalTime = pingInfo.timestamp - startTime;
			PORTC ^= _BV(PORTC5);
			
			replies++;

			printf_P(PSTR("Ping time: %ums\n"), totalTime);

			printf_P(PSTR("Signal strength: %ddBm\n"), pingInfo.rssi);

			// Print out ping contents
			printf_P(PSTR("Data from server: "));
			for(uint8_t i=0;i<sizeof(pingInfo.buffer);i++)
				printf_P(PSTR("%c"), pingInfo.buffer[i]);
			puts_P(PSTR(""));
		}

		printf_P(PSTR("Totals: %lu Sent, %lu Replies, %lu Timeouts, %lu Invalid\n------\n"), sent, replies, timeouts, invalids);

		_delay_ms(1000);					
	}
}

ISR(TIMER0_COMPA_vect)
{
	++milliseconds;
}
