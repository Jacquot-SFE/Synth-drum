#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "Synth-DrumHeart.h"

// GUItool: begin automatically generated code
AudioSynthDrumHeart      drum1;
AudioEffectMultiply      mult1;

AudioSynthDrumHeart      drum2;
AudioEffectMultiply      mult2;

AudioSynthDrumHeart      drum3;
AudioEffectMultiply      mult3;

AudioSynthDrumHeart      drum4;
AudioEffectMultiply      mult4;

AudioOutputI2SQuad       i2s1;           //xy=968,448


AudioConnection          patchCord10(drum1, 0, mult1, 0);
AudioConnection          patchCord11(drum1, 1, mult1, 1);

AudioConnection          patchCord20(drum2, 0, mult2, 0);
AudioConnection          patchCord21(drum2, 1, mult2, 1);

AudioConnection          patchCord30(drum3, 0, mult3, 0);
AudioConnection          patchCord31(drum3, 1, mult3, 1);

AudioConnection          patchCord40(drum4, 0, mult4, 0);
AudioConnection          patchCord41(drum4, 1, mult4, 1);

AudioConnection          patchCord50(mult1, 0, i2s1, 0);
AudioConnection          patchCord60(mult2, 0, i2s1, 1);
AudioConnection          patchCord70(mult3, 0, i2s1, 2);
AudioConnection          patchCord80(mult4, 0, i2s1, 3);

AudioControlSGTL5000     sgtl5000_1;     //xy=906,517
AudioControlSGTL5000     sgtl5000_2;     //xy=906,517
// GUItool: end automatically generated code


void trigger(uint32_t channel)
{
 
  AudioNoInterrupts();
  switch(channel & 0x03)
  {
  case 0:
    drum1.noteOn();
    break;
  case 1:
    drum2.noteOn();
    break;
  case 2:
    drum3.noteOn();
    break;
  case 3:
    drum4.noteOn();
    break;
  }
  AudioInterrupts();
}


static uint32_t next;
static uint32_t count;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  // audio library init
  AudioMemory(15);

  next = millis() + 1000;
  count = 0;

  // read panel before we start to run
  //paramUpdate();

  drum1.frequency(500);
  drum2.frequency(500);
  drum3.frequency(500);
  drum4.frequency(500);

  AudioNoInterrupts();
  
  sgtl5000_1.setAddress(LOW);
  sgtl5000_1.enable();
  //sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.volume(0.5);

  // Enable the second audio shield, select input, and enable output
  sgtl5000_2.setAddress(HIGH);
  sgtl5000_2.enable();
  //sgtl5000_2.inputSelect(myInput);
  sgtl5000_2.volume(0.5);

  AudioInterrupts();

}

void loop() {
  // put your main code here, to run repeatedly:

  if(millis() == next)
  {
    next = millis() + 1000;

    trigger(count);
    count++;

    Serial.print("Diagnostics: max: ");
    Serial.print(" ");
    Serial.print(AudioProcessorUsageMax());
    Serial.print(" ");
    Serial.println(AudioMemoryUsageMax());
    AudioProcessorUsageMaxReset();
  }
}
