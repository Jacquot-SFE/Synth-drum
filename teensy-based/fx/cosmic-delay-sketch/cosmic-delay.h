
#pragma once
 
#ifndef _cosmic_delay_h_
#define _cosmic_delay_h_


#include "AudioStream.h"

class AudioEffectCosmicDelay   : public AudioStream
{
public:
	AudioEffectCosmicDelay   (void) : AudioStream(2, inputQueueArray) 
	{
    buffer_length = 0;
	}
	
	virtual void update(void);

	// Set the parameters
	void setbuf(int32_t delay_len, int16_t* delay_buf )
  {
    delayline_p = delay_buf;
    insert_index = 0;
    buffer_length = delay_len;

    for(int32_t i = 0; i < buffer_length; i++)
    {
      delayline_p[i] = 0;
    }
  };

  void inspect(void)
  {
    Serial.print(insert_index, HEX);
    Serial.print(' ');
    Serial.print(delay_delta, HEX);
    Serial.print(' ');
    Serial.println(buffer_length, HEX);
    
    };
  
private:
	audio_block_t *inputQueueArray[2];

	int16_t *delayline_p;
 
  int32_t insert_index;
  int32_t buffer_length;
  int32_t delay_delta;
};

#endif
