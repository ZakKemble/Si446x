Si4463 Radio Library for AVR and Arduino
========================================

This is a library for use with the [Si4463 radio IC from Silicon Labs](http://www.silabs.com/products/wireless/EZRadioPRO/Pages/si446x.aspx). The Si4463 is used in a number of pre-made modules like the [HopeRF RFM26W](http://www.hoperf.com/rf_transceiver/modules/RFM26W.html) and [Dorji_Com DRF4463F](https://www.tindie.com/products/DORJI_COM/433mhz-wireless-rf-si4463-transceiver-module/). The whole range of Si446x transceivers should also work with this library.

http://blog.zakkemble.co.uk/si4463-radio-library-avr-arduino/

Documentation
-------------

[Doxygen pages](http://zkemble.github.io/Si446x/)

Installing
----------

### Arduino
Copy the `arduino/Si446x/` folder to your Arduino libraries folder.

Add

    #include <Si446x.h>

to the top of the sketches that use the library.

Also have a look at Si446x_config.h, that's where you can change which pins are used and stuff.

### AVR
Copy the `./Si446x` folder to your project and add

    #include "Si446x/Si446x.h"

to the source files that use the library.

Check out the examples in the examples folder.

---

Zak Kemble

contact@zakkemble.co.uk
