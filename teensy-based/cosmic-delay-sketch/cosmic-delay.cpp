 #include "cosmic-delay.h"
 //#include "utility/dspinst.h"
 
 void AudioEffectCosmicDelay::update(void)
{
	audio_block_t *block;
	int16_t *data, *end;

  //int32_t delayout, scaleddelay, currentin, summedin, scaledin;


	block = receiveWritable();
	if (!block) return;

  data = block->data;
	end = block->data + AUDIO_BLOCK_SAMPLES;
  
	do 
	{
    delayline_p[insert_index] = *data;
    insert_index++;
    if(insert_index >= delay_length)
    {
      insert_index = 0;
    }

    // Try to smooth discontinuities.
    // Noise from pot/ADC results in crackles...
    if(delay_avg < delay_delta)
    {
      delay_avg++;
    }
    else if(delay_avg > delay_delta)
    {
      delay_avg--;
    }
    
    extract_index = insert_index-delay_avg;//delta;

    if(extract_index < 0)
    {
      extract_index = delay_length + extract_index;
    }

    *data = delayline_p[extract_index];
    data++;  
	} while (data < end);
	transmit(block);
	release(block);
}


