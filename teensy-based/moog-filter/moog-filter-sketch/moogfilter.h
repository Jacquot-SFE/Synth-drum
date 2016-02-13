#ifndef _moogfilter_h_
#define _moogfilter_h_
#include "AudioStream.h"
#include "utility/dspinst.h"

// #define SAMPLES_PER_MSEC (AUDIO_SAMPLE_RATE_EXACT/1000.0)
// AUDIO_SAMPLE_RATE_EXACT
//    __disable_irq();    
//    __enable_irq();    

class AudioFilterMoog : public AudioStream
{
public:
  AudioFilterMoog() : AudioStream(1, inputQueueArray) 
  {
    in1 = in2 = in3 = in4 = 0;
    out1 = out2 = out3 = out4 = 0;

    // fc expressed relative to nyquist frequency, fs/2
    //fc = 0x4000; // 1/2 fs/2 = 11 khz
    //fc = 0x2000; // 1/4 fs/2 = 5.5khz
    //fc = 0x1800;
    //fc = 0x1400;
    //fc = 0x1100;
    //fc = 0x1010;
    fc = 0x1000; //1/16 fs/2 = 2.7khz
    //fc = 0x0b00;
    //fc - 0x0600;

    // should be q3.13 for range [0, 4]?
    res = 0x7fff;
    //res = 0x7000;
  };

  void cutoff(int16_t val)
  {
    if((val >= 0) &&(val <= 0x7fff ))
    {
      fc = val;
    }
  };

  void q(int16_t val)
  {
    if((val >= 0) &&(val <= 0x7fff ))
    {
      res = val;
    }
  };
  
  using AudioStream::release;
  virtual void update(void);

private:
  audio_block_t *inputQueueArray[1];

  int16_t in1, in2, in3, in4;
  int16_t out1, out2, out3, out4;

  int16_t fc;
  int16_t res;

};

#endif


