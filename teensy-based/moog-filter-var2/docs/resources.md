# Moog VCF Research & Links

Some resources discussing DSP implementations of the Moog VCF, with an intent to find something that's portable to the 16-bit integer world of [Teensy Audio](http://www.pjrc.com/teensy/td_libs_Audio.html).

There are lots of versions floating around.  This was an attempt to classify some of them, to possibly pick one to port.

Major criteria might be:
	* Integer math
	* No dependence on external math
	* Realtime control of Fc.

## Web & Literature Survey

* Antti H's paper?  DAFX 04?

* A [paper](http://www.acoustics.ed.ac.uk/wp-content/uploads/AMT_MSc_FinalProjects/2012__Daly__AMT_MSc_FinalProject_MoogVCF.pdf) comparing DSP implementations of the Moog filter.  Contains a reasonable amount of theory and practical implementation details.
	* The ultimate takeaway: ?? 

* The Music DSP mailing list has a [large index](http://musicdsp.org/archive.php?classid=3) of filter discussions.  Sadly, the organization of the list-on-web obliterates who the original poster is, and the date of posting... 
	1. **"Moog filter"** April 2005 discussion of [Pascal Implementation](http://musicdsp.org/showArchiveComment.php?ArchiveID=196) of Antti's filter, but link to Pascal file is dead.
		1. Which references the [Csound Moog filter source code](http://www.csounds.com/udo/displayOpcode.php?opcode_id=32), written in CSound UDO.  Legible enough to take to other languages, but apparently floating point with *tanh()* calls.
		2. And includes a C implementation from rlinder, but in floating point, and using tanh().
		3. And apparently the C impl has a sample-rate related error?
	4. **"Moog VCF"** October 2004 [thread](http://musicdsp.org/showArchiveComment.php?ArchiveID=24).  Starting point is not  entirely obvious, but there's a C++ class from 2007 a bit lower, and a template (for `<float>/<double>` independence) implementation from 2014.  floating point impl. 
	5. **"Moog VCF, Variation 1"** January 2005 [thread](http://musicdsp.org/showArchiveComment.php?ArchiveID=25), C code from CSound source?.  Floating point, but no tanh, no division.  Relatively simple code?  Possibly  unstable?
	6. **"Moog VCF, Variation  2"** March 2003 [thread](http://musicdsp.org/showArchiveComment.php?ArchiveID=26).  Referencing Csound source and Antti's paper.  Floating point, but no tanh().  Claims of instability at low Fc.
	7. **"Stilson's Moog VCF Code"** June 2004 [thread](http://musicdsp.org/showArchiveComment.php?ArchiveID=145).  Code for the pd Moog~ block, floating point, .
* The [CSound source](http://www.csounds.com/udo/displayOpcode.php?opcode_id=32) mentioned above.  Oversampled, but trig-heavy.  It didn't need to run in real time!
 
* Dimitri Diakopoulos has already done a [Moog LPF survey](https://github.com/ddiakopoulos/MoogLadders) like this, and compares a number of sources.

* Timo H Tossavainen appears to be one of the guys behind all of this. He posted a [nugget](http://music.columbia.edu/pipermail/music-dsp/2000-March/037204.html) MusicDSP in 2000.  Click the "next message" link for an addenda!

* Grav's [Java Audiogroup](https://github.com/grav/audiogroup/blob/master/jvstwrapperplug/jVSTeXamples-0.9g-sources/jvst/examples/liquinth/MoogFilter.java) version, apparently an integer implementation?



## Takeaways

### tanh()

The  hyperbolic tangent is an s-shaped curve.  It's apparently being used as a distortion & saturation function, and trig accuracy isn't terribly critical.  There are apparently some algebraic approximations or LUT implementations that are faster than the c math library.

## Testing notes?

### Attempt #1

My first try will be the variation 2 filter...let's go cook some headphones.

Update (2/15/2016): it's mostly working.

* DSP load isn't too heavy...maybe 5% of available at 72 MHz, non-optimized.
* Implementation is naive & easy.  Float constants were translated to signed 16 bit integers, math translated to use it.
	* there's a spreadsheet in the directory that contains the   
* This one was selected because the overall flow was the simplest, and it had no tanh().
* Commenter on that old thread complained about instability at low cutoffs...I'm experiencing that.below about 0xf00 it seems to be overflowing really painfully and generating white noise. 
	* Just above that boint, it looks like we're running out of bits somewhere?  As the cutoff drops, we see the sawtooth break into a number of discrete steps - 4, then 3, then 2, then bammo.
	* Appears to be independent of Q.   
* Cutoff parameter scaling was removed...f in the calcs is the pot position.
* Last, i was trying to add a little more resolution by making parameters into 16-bit *unsigned* ints...not entirely there yet.
 
### Attempt #2
 
Trying the variation 1 filter, adapting what I learned in the var #2 implementation...  

Still no tanh(), but not written as a C function...

So far, stable at low freq, but Q is underwhelming.  Doesn't mean it's not in my translation... 