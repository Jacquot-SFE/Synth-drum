#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "synth_simple_drum.h"

// GUItool: begin automatically generated code
AudioSynthSimpleDrum     drum1;
AudioOutputI2S           i2s1;           //xy=968,448

AudioConnection          patchCord5(drum1, 0, i2s1, 0);
AudioConnection          patchCord6(drum1, 0, i2s1, 1);

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
  uint16_t len, bend, pitch, sec;

  pitch = analogRead(A1);
  sec = analogRead(A2);
  bend = analogRead(A3);
  len = analogRead(A6);

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
  drum1.second(sec);

}

static uint32_t next;


void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

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

  static uint32_t num = 0;

  paramUpdate();

  if(millis() == next)
  {
    next = millis() + 250;

    num++;
    if((num % 2) == 0)
    {
          trigger();
    }

    Serial.print("Diagnostics: ");
    Serial.print(AudioProcessorUsageMax());
    Serial.print(" ");
    Serial.println(AudioMemoryUsageMax());
    AudioProcessorUsageMaxReset();
  }
}
