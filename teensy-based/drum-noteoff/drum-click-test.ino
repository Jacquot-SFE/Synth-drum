elapsedMillis tempo;

//#include "synth_simple_drum.h"
#include <SD.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SerialFlash.h>


// GUItool: begin automatically generated code
AudioSynthSimpleDrum drum1; //xy=115,72
AudioOutputI2S i2s2; //xy=1344,44
AudioConnection patchCord1(drum1, 0, i2s2, 0);
AudioConnection patchCord2(drum1, 0, i2s2, 1);
AudioControlSGTL5000 sgtl5000_1; //xy=150,124
// GUItool: end automatically generated code

void setup()
{
  AudioMemory(20);

  sgtl5000_1.enable();
  sgtl5000_1.volume(0.35);

  AudioNoInterrupts();
  drum1.frequency(40);
  drum1.length(1000);
  //drum1.length(300);
  drum1.secondMix(0.0);
  drum1.pitchMod(1.0);
  AudioInterrupts();

  delay(1000);
}

void loop()
{
  if (tempo > 550)
  {
    drum1.noteOn();
    tempo = 0;
  }
}
