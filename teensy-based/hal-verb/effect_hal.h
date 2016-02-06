
#pragma once
 
#ifndef _effect_hal_h_
#define _effect_hal_h_


#include "AudioStream.h"

class AudioEffectHal: public AudioStream
{
public:
	AudioEffectHal(void) : AudioStream(1, inputQueueArray) 
	{
	}
	
	virtual void update(void);

	// Set the parameters
	void setCoefficients(uint32_t factor, uint32_t delay_len, int16_t* delay_buf )
  {
    factor = factor;
    negfactor = -factor;

    delayline_p = delay_buf;
    delay_index = 0;
    delay_length = delay_len;
  }
private:
	audio_block_t *inputQueueArray[1];

  int16_t factor, negfactor;
	int16_t *delayline_p;
  uint32_t delay_index;
  uint32_t delay_length;
	
};

#endif
