#ifndef _PATTERN_H_
#define _PATTERN_H_

#pragma once

#include <stdint.h>

static const uint32_t PATTERN_LEN = 16;

class Pattern
{
public: 
  Pattern();

  void setBit(uint32_t step, uint32_t bit_num);

  uint32_t getStepData(uint32_t index);
private:

  uint32_t pattern_data[PATTERN_LEN];

};



#endif // keepout
