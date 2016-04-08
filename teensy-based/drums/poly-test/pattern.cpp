#include <Arduino.h>

#include "pattern.h"

static const uint32_t sequence[PATTERN_LEN] =
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
    pattern_data[i] = sequence[i];
  }
}

uint32_t Pattern::getStepData(uint32_t index)
{
  if(index >= PATTERN_LEN)
  {
    Serial.println("Overflow in pattern req");
    index %= PATTERN_LEN; 
  }
  
  return pattern_data[index];
}


