
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
	void setCoefficients(uint32_t fac, uint32_t delay_len, int16_t* delay_buf )
  {
    uint32_t i;
    
    factor = fac;
    negfactor = -fac;

    delayline_p = delay_buf;
    delay_index = 0;
    delay_length = delay_len;

    for(i = 0; i < delay_length; i++)
    {
      delayline_p[i] = 0;
    }
  };

  void inspect(void)
  {
    Serial.print(factor);
    Serial.print(' ');
    Serial.print(negfactor);
    Serial.print(' ');
    //Serial.print(delayline_p, HEX);
    //Serial.print(' ');
    Serial.print(delay_index, HEX);
    Serial.print(' ');
    Serial.println(delay_length);
    
    };
  
private:
	audio_block_t *inputQueueArray[1];

  volatile int32_t factor, negfactor;
	int16_t *delayline_p;
  uint32_t delay_index;
  uint32_t delay_length;
	
};

#endif
