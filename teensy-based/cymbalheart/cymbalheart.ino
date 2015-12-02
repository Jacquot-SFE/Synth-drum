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
AudioEffectMultiply      mult1;

AudioOutputI2S           i2s1;           //xy=968,448

AudioConnection          patchCord1(clat1, 0, mult1, 0);
AudioConnection          patchCord2(decay1, 0, mult1, 1);
AudioConnection          patchCord3(mult1, 0, i2s1, 0);
AudioConnection          patchCord4(mult1, 0, i2s1, 1);


AudioControlSGTL5000     sgtl5000_1;     //xy=906,517
// GUItool: end automatically generated code

void trigger(uint16_t val)
{
  AudioNoInterrupts();
  decay1.length(val<<2);
  decay1.noteOn();
  AudioInterrupts();

  Serial.print("Trigger: ");
  Serial.println(val);
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
    trigger(val);

    holdoff = immed + 50;
  }

  last = val;

  
}



void paramUpdate()
{
  uint16_t len, pitchA, pitchB;

  uint16_t mixadc, q, filtmod;
  uint16_t shape, cutoff, trig;
  float    mix;

  len = analogRead(A1);//on audion board...
  trig = analogRead(A14);
#if 0
  pitchA = analogRead(A0);
  pitchB = analogRead(A2);
  trig   = analogRead(A14);

  Serial.print("Analog: ");
  Serial.print(len, HEX);
  Serial.print(" ");
  Serial.println(bend, HEX);
  Serial.print(" ");
  Serial.println(pitch, HEX);
#endif

  //drum1.length((len * 2) + 50); // 50 .. 2097 mSec
//  drum2.length((len * 2) + 50); // 50 .. 2097 mSec
  //drum1.pitchMod(bend);
  //drum1.frequency(30 + (pitchA>>1));
  //drum2.frequency(30 + (pitchB>>1));

#if 0
  drum1.waveshape(shape >> 8);
  
  filter1.frequency(30 + (cutoff>>1)); //(30 .. 541)
  filter1.resonance(0.7 + (((float)q * 4.3)/ 1024.0));// (0.7 .. 5.0)

  filter1.octaveControl( ((float)filtmod * 7.0) /1024.0);// (0 .. 7.0)

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

  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);

  AudioInterrupts();

}

void loop() {
  // put your main code here, to run repeatedly:

  //Serial.println("Loop");


  paramUpdate();

  if(millis() == next)
  {
    next = millis() + 1000;

    //trigger(500);

    Serial.print("Diagnostics: max: ");
    Serial.print(AudioProcessorUsageMax());
    Serial.print(" ");
    Serial.println(AudioMemoryUsageMax());
    AudioProcessorUsageMaxReset();

    Serial.print("Array ");
    for(uint32_t i = 0; i <6; i++)
    {
      Serial.print(" ");
      Serial.print(clat1.values[i]);
    }

  }
}
