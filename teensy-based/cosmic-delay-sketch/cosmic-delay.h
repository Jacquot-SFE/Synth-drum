
#pragma once
 
#ifndef _cosmic_delay_h_
#define _cosmic_delay_h_


#include "AudioStream.h"

class AudioEffectCosmicDelay   : public AudioStream
{
public:
	AudioEffectCosmicDelay   (void) : AudioStream(1, inputQueueArray) 
	{
	}
	
	virtual void update(void);

	// Set the parameters
	void setbuf(int32_t delay_len, int16_t* delay_buf )
  {
    delayline_p = delay_buf;
    insert_index = 0;
    delay_length = delay_len;

    for(int32_t i = 0; i < delay_length; i++)
    {
      delayline_p[i] = 0;
    }
  };

  void delaylen(int16_t val)
  {
    int32_t new_delta;

    new_delta = (val * (delay_length >>1)) >> 14;
    delay_delta = new_delta;
    
    //if(val < delay_length)
    //  delay_delta = val;
      
  }

  void inspect(void)
  {
    Serial.print(insert_index, HEX);
    Serial.print(' ');
    Serial.print(delay_delta, HEX);
    Serial.print(' ');
    Serial.println(delay_length, HEX);
    
    };
  
private:
	audio_block_t *inputQueueArray[1];

	int16_t *delayline_p;
 
  int32_t insert_index;
  int32_t extract_index;
  int32_t delay_length;
  int32_t delay_delta;
  int32_t delay_avg;
	
};

#endif
