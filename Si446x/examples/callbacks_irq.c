/*
 * Project: Si4463 Radio Library for AVR and Arduino (Callbacks example)
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2017 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/si4463-radio-library-avr-arduino/
 */

/*
 * Example showing all callbacks and turning IRQ off and on
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

// The SI446X_CB_RXCOMPLETE callback is ran once the packet has been received and is valid
// This callback cannot be disabled
void SI446X_CB_RXCOMPLETE(uint8_t length, int16_t rssi)
{
	Si446x_RX(CHANNEL);
	printf_P(PSTR("Got packet (Len: %hhu | RSSI: %d)\n"), length, rssi);
}

// The SI446X_CB_RXINVALID callback is ran once the packet has been received, but was corrupted (CRC failed)
// This callback cannot be disabled
void SI446X_CB_RXINVALID(int16_t rssi)
{
	Si446x_RX(CHANNEL);
	printf_P(PSTR("Packet CRC failed (RSSI: %d)\n"), rssi);
}

// If SI446X_CBS_RXBEGIN is enabled with Si446x_setupCallback() then the SI446X_CB_RXBEGIN callback is ran when the beginning of a new packet is detected (after a valid preamble and sync)
void SI446X_CB_RXBEGIN(int16_t rssi)
{
	printf_P(PSTR("Incoming packet (RSSI: %d)\n"), rssi);
}

// If SI446X_CBS_SENT is enabled with Si446x_setupCallback() then the SI446X_CB_SENT callback is ran when a packet has finished transmitting
void SI446X_CB_SENT(void)
{
	puts_P(PSTR("Packet sent"));
}

// If SI446X_WUT_RUN is enabled in Si446x_setupWUT() then the SI446X_CB_WUT callback is ran each time the wakeup timer expires.
void SI446X_CB_WUT(void)
{
	puts_P(PSTR("Wakeup timer"));
}

// If SI446X_WUT_LOWBATT is enabled in Si446x_setupWUT() then the supply voltage is automatically tested each time the wakeup timer expires.
// If the supply voltage is below the value set by Si446x_setLowBatt() then the SI446X_CB_LOWBATT callback is ran.
void SI446X_CB_LOWBATT(void)
{
	puts_P(PSTR("Low battery"));
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
	Si446x_setLowBatt(3000); // Set low battery voltage to 3000mV
	Si446x_setupWUT(1, 8192, 0, SI446X_WUT_RUN | SI446X_WUT_BATT); // Run WUT and check battery every 2 seconds

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
		// Transmit some data every 500ms
		_delay_ms(500);

		Si446x_TX(testData, sizeof(testData), CHANNEL, SI446X_STATE_RX); // Transmit and go to receive mode once sent
		testData[0]++;

		// We're about to print some stuff to serial, however the callbacks also print to serial. The callbacks are ran from an interrupt which could run at any time (like in the middle of printing out the serial message below).
		// To make sure the callbacks don't run we temporarily turn the radio interrupt off.
		// When communicating with other SPI devices on the same bus as the radio then you should also wrap those sections in an SI446X_NO_INTERRUPT() block, this will stop the Si446x interrupt from running and trying to use the bus at the same time.

		SI446X_NO_INTERRUPT()
		{
			// Print the message
			puts_P(PSTR("Packet send begin"));
		}
		// Radio interrupt is now back on, any callbacks waiting will now run
	}
}
