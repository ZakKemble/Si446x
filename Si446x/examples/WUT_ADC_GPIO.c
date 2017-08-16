/*
 * Project: Si4463 Radio Library for AVR and Arduino (WUT, ADC and GPIO example)
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2017 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/si4463-radio-library-avr-arduino/
 */

/*
 * WUT (Wakeup timer), ADC and GPIO operations
 *
 * Here the wakeup timer is used to initiate a battery and temperature reading.
 * Every 500ms the GPIO0 output state is toggled between HIGH and LOW and GPIO input values are read.
 * If GPIO1 is LOW then a message is printed.
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

static volatile uint32_t milliseconds;
static volatile uint8_t wut;
static volatile uint8_t lowbatt;

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
	(void)(length);
	(void)(rssi);
}

void SI446X_CB_RXINVALID(int16_t rssi)
{
	(void)(rssi);
}

void SI446X_CB_WUT(void)
{
	wut = 1;
}

void SI446X_CB_LOWBATT(void)
{
	lowbatt = 1;
}

static uint32_t millis(void)
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
	
	Si446x_setLowBatt(3000); // Set low battery voltage to 3000mV
	Si446x_setupWUT(0, 16384, 0, SI446X_WUT_RUN | SI446X_WUT_BATT); // Run WUT and check battery every 2 seconds
	Si446x_writeGPIO(SI446X_GPIO1, SI446X_GPIO_MODE_INPUT | SI446X_GPIO_PULL_EN); // Set GPIO 1 as INPUT with PULLUP
	Si446x_sleep();

	// Global interrupts on
	sei();

	uint32_t lastRead = 0;
	uint8_t gpState = 0;

	while(1)
	{
		if(wut)
		{
			wut = 0;
			uint16_t batt = Si446x_adc_battery(); // Read supply voltage
			float temp = Si446x_adc_temperature(); // Read temperature
			Si446x_sleep(); // Go to sleep
			printf_P(PSTR("Battery: %umV | Temp: %dC\n"), batt, (int16_t)temp); // Print values
		}

		// Print a message when the supply voltage is below the value set by Si446x_setLowBatt()
		if(lowbatt)
		{
			lowbatt = 0;
			Si446x_sleep();
			puts_P(PSTR("Low battery!"));
		}
		
		if(millis() - lastRead >= 500)
		{
			lastRead = millis();
			
			// Toggle GPIO 0 output state
			if(gpState)
				Si446x_writeGPIO(SI446X_GPIO0, SI446X_GPIO_MODE_DRIVE1);
			else
				Si446x_writeGPIO(SI446X_GPIO0, SI446X_GPIO_MODE_DRIVE0);
			gpState = !gpState;

			// Read GPIO input values
			uint8_t states = Si446x_readGPIO();

			// If GPIO1 is LOW then print a message
			if(!(states & _BV(SI446X_GPIO1)))
				puts_P(PSTR("GPIO1 Active!"));
			
			// Go to sleep
			Si446x_sleep();
		}
	}
}

ISR(TIMER0_COMPA_vect)
{
	++milliseconds;
}
