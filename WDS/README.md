Various configs
===============

| Parameter           | config_normal | config_longrange | config_longrange_ook | config_longrange_500 | config_highspeed |
| ------------------- | ------------- | ---------------- | -------------------- | -------------------- | ---------------- |
| Modulation          | 2GFSK         | 2FSK             | OOK                  | 2FSK                 | 4GFSK            |
| Base frequency      | 433 MHz       | 433 MHz          | 433 MHz              | 433 MHz              | 433 MHz          |
| Channel spacing     | 250 KHz       | 250 KHz          | 250 KHz              | 250 KHz              | 250 KHz          |
| Data rate           | 100 Kbps      | 4.8 Kbps         | 4.8 Kbps             | 0.5 Kbps             | 500 Kbps         |
| Deviation           | 50 KHz        | 2.4 KHz          | -                    | 1 KHz                | 60 KHz           |
| RX Bandwidth        | 150 KHz       | 8 KHz            | 40 KHz               | 8 KHz                | 700 KHz          |
| CRC                 | CRC-16 (IBM)  | CRC-16 (IBM)     | CRC-16 (IBM)         | CRC-16 (IBM)         | CRC-16 (IBM)     |
| Data whitening      | Enabled       | Enabled          | Enabled              | Enabled              | Enabled          |
| Manchester encoding | Disabled      | Disabled         | Disabled             | Disabled             | Disabled         |
| Crystal frequency   | 30 MHz        | 30 MHz           | 30 MHz               | 30 MHz               | 30 MHz           |


Use radio_config.pl (or radio_config.exe if you're on Windows without a Perl install) to process the header files that WDS generated so the library can use them.

`perl radio_config.pl radio_config_Si4463.h` will take radio_config_Si4463.h, process it, and output radio_config.h

On Windows the .h files can be dragged on top of radio_config.exe which will then output a processed radio_config.h

The range of the config_longrange_500 config was tested by driving around a fairly flat village/country area, the longest distance a valid packet was received at was 1.83 km (1.14 miles) and 2.02 km (1.26 miles) for an invalid packet.
