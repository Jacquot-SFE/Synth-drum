# Synth-drum
An AVR-based drum synthesizer.  A digital Synare, if you will.

A projet for me to test out Atmel Studio interfacing with a SparkFun RedBoard using an AtmelICE (PCBA Only version).

So far, so good, but a couple quirks discovered:

* the cap from the FTDI to the reset line needs to be removed for debug wire to work.  An Arduino Pro might be a better choice for applications like this.
* It's not booting yet.  I think the bootloader doesn't find a valid program signature, so doesn't initialize it.

This is very much "WIP," and could change significantly in the near future.

Once things stabilize, I'll add some notes about the extra hardware added. 