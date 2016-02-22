 #include "cosmic-delay.h"
 //#include "utility/dspinst.h"

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

 void AudioEffectCosmicDelay::update(void)
{
	audio_block_t *audioblock, *controlblock;
	int16_t *data, *end, *ctrl;

#ifdef INTERPOLATE
  int32_t next;
  int16_t calc;
#endif  

	audioblock = receiveWritable(0);
  if (!audioblock) return;

  controlblock = receiveReadOnly(1);
  if (!controlblock) return;

  data = audioblock->data;
	end = audioblock->data + AUDIO_BLOCK_SAMPLES;

  ctrl = controlblock->data;
  
	do 
	{
    delayline_p[insert_index] = *data;
    insert_index++;
    if(insert_index >= buffer_length)
    {
      insert_index = 0;
    }

#ifdef INTERPOLATE
    interp_delta =  (buffer_length * (*ctrl));
    delay_delta = interp_delta >> 15;   // MSB's for delay len
    interp_delta &= 0x7fff;             //LSBs for interp distance
#else
    delay_delta = (buffer_length * (*ctrl)) >> 15;
#endif
      
    extract_index = insert_index-delay_delta;

    if(extract_index < 0)
    {
      extract_index = buffer_length + extract_index;
    }

#ifdef INTERPOLATE
    // Use the fractional part to interpolate between samples
    next = extract_index + 1;
    if(next >= buffer_length)
    {
      next = 0;
    }

    calc = delayline_p[next] -  delayline_p[extract_index];

    calc = (calc * interp_delta )>>15;
    calc += delayline_p[extract_index];

    *data = calc;
#else
    *data = delayline_p[extract_index];
#endif    

    data++;  
    ctrl++;
	} while (data < end);
	transmit(audioblock);
	release(audioblock);
  release(controlblock);
}


