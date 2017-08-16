/*
 * Project: Si4463 Radio Library for AVR and Arduino
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2017 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/si4463-radio-library-avr-arduino/
 */

#include <avr/io.h>
#include "Si446x_spi.h"

void spi_init()
{
// These register settings are for the ATmega48/88/168/328
// Pins may need adjusting on other microcontrollers

	DDRB |= _BV(DDB2)|_BV(DDB3)|_BV(DDB5); // SS, MOSI and SCK as outputs
	PORTB |= _BV(PORTB4)|_BV(PORTB2); // Pullup enable on MISO, output HIGH on SS
	PORTB &= ~(_BV(PORTB3)|_BV(PORTB5)); // Make sure MOSI and SCK are outputting LOW

	// Max SPI clock of Si446x is 10MHz
	SPCR = _BV(SPE)|_BV(MSTR); // SPI enable + Master mode
	SPSR = _BV(SPI2X); // Double speed
}
