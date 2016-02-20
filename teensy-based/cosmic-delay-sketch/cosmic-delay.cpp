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
  uint16_t control;

  //int32_t delayout, scaleddelay, currentin, summedin, scaledin;


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

    delay_delta = (buffer_length * (*ctrl)) >> 15;
      
    extract_index = insert_index-delay_delta;

    if(extract_index < 0)
    {
      extract_index = buffer_length + extract_index;
    }

    *data = delayline_p[extract_index];
    data++;  
    ctrl++;
	} while (data < end);
	transmit(audioblock);
	release(audioblock);
  release(controlblock);
}


