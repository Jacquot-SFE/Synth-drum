#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "effect_hal.h"
#include "Synth-Decay.h"

// test signal
AudioSynthNoiseWhite   noisegen;
AudioSynthDecay        noisedecay;
AudioEffectMultiply    noisemult;

AudioEffectHal         hal1;

AudioOutputI2S           i2s1;           
AudioControlSGTL5000     sgtl5000_1;


// Patch 'em together
AudioConnection          patchCord01(noisegen, 0, noisemult, 0);
AudioConnection          patchCord02(noisedecay, 0, noisemult, 1);
AudioConnection          patchCord03(noisemult, 0, hal1, 0);
//AudioConnection          patchCord04(noisemult, 0, i2s1, 0);
AudioConnection          patchCord04(hal1, 0, i2s1, 0);
AudioConnection          patchCord05(noisemult, 0, i2s1, 1);


// some globals
int16_t dely1buf[10000];


static uint32_t next;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial.println("Setup");

  AudioMemory(20);

  AudioNoInterrupts();

  noisedecay.length(25);

  noisegen.amplitude(0.5);

  //                   0.5..
  hal1.setCoefficients(0x4000, 10000, dely1buf);

  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);


  AudioInterrupts();

  next = millis()+1000;

  Serial.println("Setup Complete.");

}

void loop() {
  // put your main code here, to run repeatedly:

  if(millis() > next)
  {
    next += 5000;

    noisedecay.noteOn();

    //Serial.println("triggered.");

    Serial.print("Diagnostics: ");
    Serial.print(" max, buffs: ");
    Serial.print(AudioProcessorUsageMax());
    Serial.print(" ");
    Serial.println(AudioMemoryUsageMax());
    AudioProcessorUsageMaxReset();

    
  }

}
