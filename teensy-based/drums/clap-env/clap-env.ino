#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "Effect-ClapEnv.h"
#include "Synth-Decay.h"

// GUItool: begin automatically generated code
AudioSynthNoiseWhite     noise1;         //xy=573,86
AudioEffectClapEnvelope decay1;
AudioFilterBiquad       noisefilt;
AudioSynthDecay          decay2;
AudioEffectMultiply       mult;
AudioMixer4              mix;


AudioOutputI2S           i2s1;           //xy=968,448
AudioConnection          patchCord1(noise1, 0, decay1, 0);

AudioConnection          patchCord2(noise1, 0, noisefilt, 0);
AudioConnection          patchCord002(noisefilt, 0, mult, 0);
AudioConnection          patchCord3(decay2, 0, mult, 1);

AudioConnection          patchCord4(decay1, 0, mix, 0);
AudioConnection          patchCord5(mult,   0, mix, 1);

AudioConnection          patchCord6(mix, 0, i2s1, 0);
AudioConnection          patchCord7(mix, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=906,517
// GUItool: end automatically generated code


static uint32_t next;

void updateParams()
{
  uint16_t value;
  
  value = analogRead(A3);
  //decay2.length(value);


}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  AudioMemory(15);

  next = millis() + 2000;

  AudioNoInterrupts();
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);

  decay2.length(255);

  mix.gain(0, 0.5);
  mix.gain(1, 0.2);

  noisefilt.setLowpass(0, 7500, 0.4);

  noise1.amplitude(0.5);

  AudioInterrupts();

}

void loop() {
  // put your main code here, to run repeatedly:

  updateParams();

  if( millis() >= next)
  {
    next += 2000;
    
    decay1.noteOn();
    decay2.noteOn();

    Serial.print("Diagnostics (prco/mem) ");
    Serial.print(AudioProcessorUsageMax());
    Serial.print(" ");
    Serial.println(AudioMemoryUsageMax());
    AudioProcessorUsageMaxReset();
#if 0
    Serial.print(decay1.even_increment);
    Serial.print(" ");
    Serial.print(decay1.odd_increment);
    Serial.print(" ");
    Serial.println(decay1.verb_increment);
#endif
  }
  
#if 0
  //Serial.println(decay1.increment);
  for (int i = 0 ; i < 50; i++)
  {
    Serial.println(decay1.current, HEX);
    Serial.println(decay1.state, HEX);
    delay(5);
  }
#endif



}
