#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "moogfilter.h"

/***************/

AudioSynthWaveform   gen;
AudioFilterMoog2      filter;
AudioEffectEnvelope  vca;


AudioOutputI2S       i2s1;           
AudioControlSGTL5000 sgtl5000_1;

/***************/

AudioConnection     patchCord01(gen, 0, filter, 0);
AudioConnection     patchCord02(filter, 0, vca, 0);
AudioConnection     patchCord03(vca, 0, i2s1, 0);
AudioConnection     patchCord04(vca, 0, i2s1, 1);

/***************/

uint32_t next;

int16_t qpeek, rezpeek;

/***************/

uint16_t param_update()
{
  uint16_t value;

  // ADC goves us 10 bits of data.

  value = (analogRead(A2) << 5);
  
  filter.q(value);
  
  
  value = (analogRead(A1) << 5);
  
  filter.cutoff(value);

  return(value);
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial.println("Setup");

  AudioMemory(20);

  AudioNoInterrupts();

  gen.frequency(100);
  //gen.frequency(80);
  //gen.amplitude(1.0);
  gen.amplitude(0.25);
  //gen.begin(WAVEFORM_SAWTOOTH);
  gen.begin(WAVEFORM_SQUARE);

  vca.attack(25);
  vca.decay(100);
  vca.sustain(0.5);
  vca.release(200);

  sgtl5000_1.enable();
  sgtl5000_1.volume(0.15);

  AudioInterrupts();

  next = millis() + 1000;

  Serial.println("Setup complete.");

}



void loop() {
  // put your main code here, to run repeatedly:
  static bool next_on = true;

  static uint32_t count = 0;

  uint16_t peek;

  peek = param_update();

  if(millis() > next)
  {
    next += 500;

    switch(count % 4)
    {
      case 0:
        gen.frequency(100);
        vca.noteOn();
      break;

      case 1:
        vca.noteOff();
      break;

      case 2:
        gen.frequency(150);
        vca.noteOn();
      break;

      case 3:
        vca.noteOff();
      break;
    }

    count++;

    //Serial.println(peek, HEX);
    
    Serial.print("Diagnostics: ");
    Serial.print(" max, buffs: ");
    Serial.print(AudioProcessorUsageMax());
    Serial.print(" ");
    Serial.println(AudioMemoryUsageMax());
    AudioProcessorUsageMaxReset();
    Serial.print("rezpeek: ");
    Serial.print(rezpeek, HEX);
    Serial.print(" qpeek: ");
    Serial.println(qpeek, HEX);
  }

}
