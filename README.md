# Synth-drum

**In the face of uncertainty, the source code will supersede this document.**  

The cleanliness of the project leaves a lot to be desired.  It's a scratchpad for whatever interests me at the moment.

## Some History

The AVR path turned into a red herring.  I was maxing out the CPU, and the 12-bit R2R ladder aliased something terible.

And then the Teensy Audio platform landed on my desk, which is more capable, all around.

The AVR parts of the project are still here, in the *328-based* directories.

The AVR stuff is mostly obsolete, replaced by the *teensy-based* stuff.  The Teensy is much more capable of realtime synthesis, has a decent audio I/O subsystem, and a large body of working sample code.  It's capable enough that I've got a lot more than just a Synare-based thingy working on it.
 
---

## Historical Project Notes 

An AVR-based drum synthesizer.  A digital Synare, if you will.

A projet for me to test out Atmel Studio interfacing with a SparkFun RedBoard using an AtmelICE (PCBA Only version).

So far, so good, but a couple quirks discovered:

* the cap from the FTDI to the reset line needs to be removed for debug wire to work.  An Arduino Pro might be a better choice for applications like this.
* It's not booting yet.  I think the bootloader doesn't find a valid program signature, so doesn't initialize it.

This is very much "WIP," and could change significantly in the near future.

Once things stabilize, I'll add some notes about the extra hardware added. 

---

Some inspiration drawn from tom Wiltshire's [VCADSR](http://www.electricdruid.net/index.php?page=projects.envgen7)