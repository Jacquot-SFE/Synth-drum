#ifndef _PATTERN_H_
#define _PATTERN_H_

#pragma once

#include <stdint.h>


class Pattern
{
public: 
  static const uint32_t PATTERN_LEN = 16;

  Pattern();

  bool toggleBit(uint32_t step);

  void setCurrentVoice(uint32_t);
  uint32_t getCurrentVoice(void);

  uint32_t getStepData(uint32_t index);
  bool     getVoiceBit(uint32_t step);
private:

  uint32_t pattern_data[PATTERN_LEN];

  uint32_t current_voice;
  uint32_t current_voice_mask;
  

};



#endif // keepout
