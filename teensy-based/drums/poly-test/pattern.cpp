#include <Arduino.h>

#include "pattern.h"

static const uint32_t sequence[Pattern::PATTERN_LEN] =
{
#if 0
  //shaker and toms....
  //S123OCSK
  0b01000001,
  0b00000000,
  0b10100000,
  0b10000000,
  0b00010001,
  0b00000000,
  0b10100000,
  0b10000000,
  0b01000001,
  0b01000000,
  0b10000000,
  0b10100000,
  0b00100001,
  0b00000000,
  0b10010000,
  0b10010000,
#else
  //kick, snare & hats
  0b00000101,
  0b00000100,
  0b10000100,
  0b10000101,
  0b00000110,
  0b00000100,
  0b10001001,
  0b10000010,
  0b00000101,
  0b00000100,
  0b10000100,
  0b10000100,
  0b00000110,
  0b00000100,
  0b10010100,
  0b10000100,
#endif
};


// constructor...
Pattern::Pattern()
{

  for(uint32_t i = 0; i < PATTERN_LEN; i++)
  {
    for(uint32_t j = 0; j < NUM_PATTERNS; j++)
    {
      pattern_data[j][i] = sequence[i];
    }
  }

  setCurrentVoice(0);
  setCurrentPattern(0);

}

bool Pattern::toggleBit(uint32_t index)
{
  if(index >= PATTERN_LEN)
  {
    Serial.println("Overflow in toggle");
    index %= PATTERN_LEN; 
  }

  pattern_data[current_pattern][index] ^= current_voice_mask;

  return(pattern_data[current_pattern][index] & current_voice_mask);
  
}


// Return all of the buts for the current step
uint32_t Pattern::getStepData(uint32_t index)
{
  if(index >= PATTERN_LEN)
  {
    Serial.println("Overflow in pattern req");
    index %= PATTERN_LEN; 
  }
  
  return pattern_data[current_pattern][index];
}

// Get all of the column data for a particular voice
bool Pattern::getVoiceBit(uint32_t step)
{
  if(step >= PATTERN_LEN)
  {
    Serial.println("Overflow in pattern bit");
    step %= PATTERN_LEN; 
  }
  
  return (pattern_data[current_pattern][step] & current_voice_mask);
}

void Pattern::setCurrentVoice(uint32_t num)
{
  current_voice = num;
  current_voice_mask = 1 << num;
}

uint32_t Pattern::getCurrentVoice(void)
{
  return current_voice;
}

void Pattern::setCurrentPattern(uint32_t nextpatt)
{
  if(nextpatt < NUM_PATTERNS)
  {
    current_pattern = nextpatt;
  }
}

uint32_t Pattern::getCurrentPattern()
{
  return current_pattern;
}

