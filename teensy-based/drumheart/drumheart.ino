#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "Synth-DrumHeart.h"

// GUItool: begin automatically generated code
AudioSynthNoiseWhite     noise1;         //xy=573,86
AudioSynthDrumHeart      drum1;

AudioFilterStateVariable filter1;

AudioMixer4              mixer;
AudioEffectMultiply      mult1;
AudioOutputI2S           i2s1;           //xy=968,448


AudioConnection          patchCord1(drum1, 0, mixer, 0);

AudioConnection          patchCord22(noise1, 0, filter1, 0);
AudioConnection          patchCord13(filter1, 1, mixer, 1);

AudioConnection          patchCord24(drum1, 1, filter1, 1);

AudioConnection          patchCord14(mixer, 0, mult1, 0);
AudioConnection          patchCord23(drum1, 1, mult1, 1);

AudioConnection          patchCord5(mult1, 0, i2s1, 0);
AudioConnection          patchCord6(mult1, 0, i2s1, 1);

AudioControlSGTL5000     sgtl5000_1;     //xy=906,517
// GUItool: end automatically generated code


void trigger()
{
  AudioNoInterrupts();
  drum1.noteOn();
  AudioInterrupts();
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
    drum1.frequency(30 + ((val-50)>>2));
    trigger();

    holdoff = immed + 50;
  }

  last = val;

  
}

void paramUpdate()
{
  uint16_t len, bend, pitch;

  uint16_t mixadc, q, filtmod;
  uint16_t shape, cutoff, trig;
  float    mix;

  len = analogRead(A1);
  bend = analogRead(A2);
  pitch = analogRead(A0);
  mixadc = analogRead(A6);
  q = analogRead(A7);
  filtmod = analogRead(A3);
  shape = analogRead(A10);
  cutoff = analogRead(A11);
  trig = analogRead(A14);

#if 0
  Serial.print("Analog: ");
  Serial.print(len, HEX);
  Serial.print(" ");
  Serial.println(bend, HEX);
  Serial.print(" ");
  Serial.println(pitch, HEX);
#endif

  drum1.length((len * 2) + 50); // 50 .. 2097 mSec
  drum1.pitchMod(bend);
  //drum1.frequency(30 + (pitch>>1));

  drum1.waveshape(shape >> 8);
  
  filter1.frequency(30 + (cutoff>>1)); //(30 .. 541)
  filter1.resonance(0.7 + (((float)q * 4.3)/ 1024.0));// (0.7 .. 5.0)

  filter1.octaveControl( ((float)filtmod * 7.0) /1024.0);// (0 .. 7.0)

  // 0 is osc, 1 is noise
  mix = (float)mixadc / 1024.0;
  mixer.gain(0, (1.0 - mix));
  mixer.gain(1, (mix));

  processTrig(trig);

}

static uint32_t next;


void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  pinMode(13, OUTPUT); // LED pin
  //pinMode(15, INPUT); // Volume pot pin?
  pinMode(15, INPUT); // Volume pot pin?

  // audio library init
  AudioMemory(15);

  next = millis() + 1000;

  // read panel before we start to run
  paramUpdate();

  AudioNoInterrupts();
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.75);

  noise1.amplitude(0.5);
  //sine1.frequency(1);
  //sine1.amplitude(1.0);

  filter1.frequency(100);
  filter1.resonance(4.0);
  filter1.octaveControl(3.0);  

  AudioInterrupts();

}

void loop() {
  // put your main code here, to run repeatedly:

  uint32_t a, b;
  static uint32_t max = 0;

  a = millis();
  paramUpdate();
  b = millis();

  if(b-a > max)
  {
    max = b-a;
  }

  if(millis() == next)
  {
    next = millis() + 1000;

    //trigger();

    Serial.print("Diagnostics: max: ");
    Serial.print(max);
    Serial.print(" ");
    Serial.print(AudioProcessorUsageMax());
    Serial.print(" ");
    Serial.println(AudioMemoryUsageMax());
    AudioProcessorUsageMaxReset();

    max = 0;
  }
}
