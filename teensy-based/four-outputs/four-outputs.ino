#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "synth_simple_drum.h"

// GUItool: begin automatically generated code
//AudioSynthSimpleDrum     drum1;
//AudioSynthSimpleDrum     drum2;
//AudioSynthSimpleDrum     drum3;
//AudioSynthSimpleDrum     drum4;

AudioOutputI2SQuad       i2s1;           //xy=968,448
AudioInputI2SQuad        in1;

//AudioConnection          patchCord50(drum1, 0, i2s1, 0);
//AudioConnection          patchCord60(drum2, 0, i2s1, 1);
//AudioConnection          patchCord70(drum3, 0, i2s1, 2);
//AudioConnection          patchCord80(drum4, 0, i2s1, 3);
AudioConnection          patchCord50(in1, 0, i2s1, 0);
AudioConnection          patchCord60(in1, 0, i2s1, 1);
AudioConnection          patchCord70(in1, 0, i2s1, 2);
AudioConnection          patchCord80(in1, 0, i2s1, 3);

AudioControlSGTL5000     sgtl5000_1;     //xy=906,517
AudioControlSGTL5000     sgtl5000_2;     //xy=906,517
// GUItool: end automatically generated code

#if 0
void trigger(uint32_t channel)
{

  AudioNoInterrupts();
  switch (channel & 0x03)
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
#endif

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

#if 0
  drum1.frequency(100);
  drum2.frequency(66);
  drum3.frequency(150);
  drum4.frequency(223);
#endif

  AudioNoInterrupts();

  sgtl5000_1.setAddress(LOW);
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
  sgtl5000_1.lineInLevel(15);
  sgtl5000_1.volume(0.6);
  sgtl5000_1.micGain(0); //- seems to influence noise on input?
  sgtl5000_1.adcHighPassFilterDisable(); //- seems to influence noise on input?
  sgtl5000_1.autoVolumeDisable();
  //sgtl5000_1.audioProcessorDisable();

  // Enable the second audio shield, select input, and enable output
  sgtl5000_2.setAddress(HIGH);
  sgtl5000_2.enable();
  sgtl5000_2.inputSelect(AUDIO_INPUT_LINEIN);
  sgtl5000_2.lineInLevel(15);
  sgtl5000_2.volume(0.6);
  sgtl5000_2.micGain(0); //- seems to influence noise on input?
  sgtl5000_2.adcHighPassFilterDisable(); //- seems to influence noise on input?
  sgtl5000_2.autoVolumeDisable();
  //sgtl5000_2.audioProcessorDisable();

  
  AudioInterrupts();

}

void loop() {
  // put your main code here, to run repeatedly:

  if (millis() == next)
  {
    next = millis() + 1000;

    //trigger(count);
    count++;

    Serial.print("Diagnostics: max: ");
    Serial.print(" ");
    Serial.print(AudioProcessorUsageMax());
    Serial.print(" ");
    Serial.println(AudioMemoryUsageMax());
    AudioProcessorUsageMaxReset();
  }
}
