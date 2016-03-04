#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "Synth-Clatter.h"
#include "Synth-Decay.h"

// GUItool: begin automatically generated code
AudioSynthClatter      clat1;
AudioSynthDecay        decay1;
AudioSynthDecay        belldecay;
AudioFilterStateVariable filt1;
AudioFilterStateVariable filt2;
AudioEffectMultiply      mult1;
AudioEffectMultiply      bellmult;

AudioMixer4              mixer;

AudioOutputI2S           i2s1;           //xy=968,448

AudioConnection          patchCord01(clat1, 0, filt1, 0);
AudioConnection          patchCord02(decay1, 0, filt1, 1);
AudioConnection          patchCord03(decay1, 0, filt2, 1);
AudioConnection          patchCord04(filt1, 2, filt2, 0);//0 = lp, 1 = bp, 2 = hp
AudioConnection          patchCord05(filt2, 1, mult1, 0);//0 = lp, 1 = bp, 2 = hp
AudioConnection          patchCord06(decay1, 0, mult1, 1);
AudioConnection          patchCord07(mult1, 0, mixer, 0);

AudioConnection          patchCord08(clat1, 1, bellmult, 0);
AudioConnection          patchCord09(belldecay, 0, bellmult, 1);
AudioConnection          patchCord10(bellmult, 0, mixer, 1);

AudioConnection          patchCord11(mixer, 0, i2s1, 0);
AudioConnection          patchCord12(mixer, 0, i2s1, 1);

AudioControlSGTL5000     sgtl5000_1;     //xy=906,517
// GUItool: end automatically generated code

void trigger(uint16_t val)
{
  AudioNoInterrupts();
  decay1.length(val);
  decay1.noteOn();
  AudioInterrupts();

  Serial.print("Trigger: ");
  
  Serial.println(val);
}

void belltrigger()
{
  belldecay.noteOn();
}

void processTrig(uint16_t val)
{
  static uint16_t last = 0;
  static int32_t  holdoff, immed;

#if 0
  Serial.print("trig peek: ");
  Serial.print(val);
  Serial.print(" ");
  Serial.println(last);
#endif
  immed = millis();
  
  if( (val > 50) && 
      (last < 50) &&
      (immed > holdoff))
  {
    //drum1.frequency(30 + ((val-50)>>2));
    trigger(val-50);

    holdoff = immed + 50;
  }

  last = val;

  
}



void paramUpdate()
{
  uint16_t cutoff, q, filtmod;
  uint16_t cutoff2, q2, filtmod2;
  uint16_t trig;
  float    mix;

  //len = analogRead(A1);//on audio board...
  cutoff = analogRead(A0);
  q      = analogRead(A2);
  filtmod = analogRead(A3);
  cutoff2 = analogRead(A6);
  q2      = analogRead(A7);
  filtmod2 = analogRead(A10);
  trig = analogRead(A14);

#if 0
  Serial.print("Analog: ");
  Serial.print(cutoff, HEX);
  Serial.print(" ");
  Serial.println(q, HEX);
  Serial.print(" ");
  Serial.println(filtmod, HEX);
#endif

  
  filt1.frequency(30 + (cutoff)); //(30 .. 541)
  filt1.resonance(0.7 + (((float)q * 4.3)/ 1024.0));// (0.7 .. 5.0)
  filt1.octaveControl( ((float)filtmod * 7.0) /1024.0);// (0 .. 7.0)

  filt2.frequency(30 + (cutoff2)); //(30 .. 541)
  filt2.resonance(0.7 + (((float)q2 * 4.3)/ 1024.0));// (0.7 .. 5.0)
  filt2.octaveControl( ((float)filtmod2 * 7.0) /1024.0);// (0 .. 7.0)

#if 0
// 0 is osc, 1 is noise
  mix = (float)mixadc / 1024.0;
  mixer.gain(0, (1.0 - mix));
  mixer.gain(1, (mix));
#endif
  processTrig(trig);

}
static uint32_t next;


void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial.println("Setup");

  pinMode(13, OUTPUT); // LED pin
  //pinMode(15, INPUT); // Volume pot pin?
  pinMode(15, INPUT); // Volume pot pin?

  // audio library init
  AudioMemory(15);

  next = millis() + 1000;

  // read panel before we start to run
  //paramUpdate();

  AudioNoInterrupts();

  //decay1.length(1000);

  belldecay.length(500);

  mixer.gain(0, 0.5);
  mixer.gain(1, 0.5);


  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);

  AudioInterrupts();

}

void loop() {
  // put your main code here, to run repeatedly:

  static uint8_t count = 0;

  //Serial.println("Loop");



  //paramUpdate();

  if(millis() == next)
  {
    count++;
    
    next = millis() + 2000;

    if(count & 0x01)
    {
      belltrigger();
    }
    else
    {
      trigger(500);
    }

    Serial.print("Diagnostics: max: ");
    Serial.print(AudioProcessorUsageMax());
    Serial.print(" ");
    Serial.println(AudioMemoryUsageMax());
    AudioProcessorUsageMaxReset();
#if 0
    Serial.print("Array ");
    for(uint32_t i = 0; i <6; i++)
    {
      Serial.print(" ");
      Serial.print(clat1.values[i]);
    }
#endif
  }
}
