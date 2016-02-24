 #include "cubic-distort.h"
 #include "utility/dspinst.h"

/*
 * Inspired by;
 * http://musicdsp.org/showArchiveComment.php?ArchiveID=154
 * 
 * But some details changed for Teensy Audio.
 * No feedback within the class, just done externally
 * so we can add filters and stuff to the loop.
 * 
 * Delay time is an input to the block, and functions from 0 to 0x7fff,
 * scaling the block length accordingly.
 */
 
 void AudioEffectCubicDistort::update(void)
{
	audio_block_t *audioblock;
	int16_t *data, *end;

  int32_t calc, temp;


	audioblock = receiveWritable(0);
  if (!audioblock) return;

  data = audioblock->data;
	end = audioblock->data + AUDIO_BLOCK_SAMPLES;

	do 
	{
    calc = *data;
    calc <<= 16;

    // Cube
    temp = multiply_32x32_rshift32_rounded(calc, calc);
    temp = multiply_32x32_rshift32_rounded(calc, temp);

    // times 1/3 - peaks at 0.666
    //temp = multiply_32x32_rshift32_rounded(calc, 0x2aaaaaaa);

    // times 1/5 - peaks at 0.8
    temp = multiply_32x32_rshift32_rounded(calc, 0x19999999);

    // times 1/6 - peaks at 0.833
    //temp = multiply_32x32_rshift32_rounded(calc, 0x15555555);

    *data = (calc - temp)>> 16;
    data++;  
	} while (data < end);
	transmit(audioblock);
	release(audioblock);
}


