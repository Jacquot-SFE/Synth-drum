
#pragma once
 
#ifndef _cubic_distort_h_
#define _cubic_distort_h_


#include "AudioStream.h"

class AudioEffectCubicDistort   : public AudioStream
{
public:
	AudioEffectCubicDistort   (void) : AudioStream(1, inputQueueArray) 
	{
	}
	
	virtual void update(void);

  
private:
	audio_block_t *inputQueueArray[1];

};

#endif
