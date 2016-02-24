#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "Synth-DrumHeart.h"

// GUItool: begin automatically generated code
AudioSynthDrumHeart      drum1;

AudioEffectMultiply      mult1;
AudioOutputI2S           i2s1;           //xy=968,448


AudioConnection          patchCord1(drum1, 0, mult1, 0);
AudioConnection          patchCord24(drum1, 1, mult1, 1);

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


void paramUpdate()
{
  uint16_t len, bend, pitch;

  uint16_t mixadc, q, filtmod;
  uint16_t shape, cutoff, trig;
  float    mix;

  len = analogRead(A1);
  bend = analogRead(A2);
  pitch = analogRead(A0);
//  shape = analogRead(A10);

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
  drum1.frequency(30 + (pitch>>1));

//  drum1.waveshape(shape >> 8);
  
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
  sgtl5000_1.volume(0.5);

  AudioInterrupts();

}

void loop() {
  // put your main code here, to run repeatedly:

  uint32_t a, b;
  static uint32_t max = 0;
  static uint32_t num = 0;

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

    trigger();

    Serial.print("Diagnostics: max: ");
    Serial.print(max);
    Serial.print(" ");
    Serial.print(AudioProcessorUsageMax());
    Serial.print(" ");
    Serial.println(AudioMemoryUsageMax());
    AudioProcessorUsageMaxReset();

    max = 0;

    num++;
    if((num % 8) == 0)
    {
      drum1.second(true);
    }
    if((num % 16) == 0)
    {
      drum1.second(false);
    }


  }
}
