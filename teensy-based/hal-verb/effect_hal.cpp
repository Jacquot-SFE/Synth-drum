 #include "effect_hal.h"
 #include "utility/dspinst.h"
 
 void AudioEffectHal::update(void)
{
	audio_block_t *block;
	int16_t *data, *end;

  int16_t delayout, scaleddelay, currentin, summedin, scaledin;


	block = receiveWritable();
	if (!block) return;

  data = block->data;
	end = block->data + AUDIO_BLOCK_SAMPLES;
  
	do 
	{
    currentin = *data;

    //*data = 0;//currentin;
#if 0

    *data = currentin;
#else
    delayout = delayline_p[delay_index];

    scaleddelay = multiply_16bx16b(delayout, factor) >> 16;
    summedin = scaleddelay + currentin;

    delayline_p[delay_index] = summedin;
    delay_index++;
    if(delay_index > delay_length)
    {
      delay_index = 0;
    }
    
    scaledin = multiply_16bx16b(summedin, negfactor) >> 16;

    //delayline_p[delay_index] = summedin;

    *data = scaledin + delayout;
#endif    
    data++;  
	} while (data < end);
	transmit(block);
	release(block);
}


