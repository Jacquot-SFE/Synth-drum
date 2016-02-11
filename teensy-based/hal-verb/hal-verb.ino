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
AudioEffectHal         hal2;
AudioEffectHal         hal3;
AudioEffectHal         hal4a;
AudioEffectHal         hal5a;
AudioEffectHal         hal4b;
AudioEffectHal         hal5b;

AudioMixer4            mixL;
AudioMixer4            mixR;

AudioOutputI2S           i2s1;           
AudioControlSGTL5000     sgtl5000_1;


// Patch 'em together
AudioConnection          patchCord01(noisegen, 0, noisemult, 0);
AudioConnection          patchCord02(noisedecay, 0, noisemult, 1);
AudioConnection          patchCord03(noisemult, 0, hal1, 0);
AudioConnection          patchCord04(noisemult, 0, mixL, 0);
AudioConnection          patchCord05(noisemult, 0, mixR, 0);

AudioConnection          patchCord06(hal1, 0, hal2, 0);
AudioConnection          patchCord07(hal2, 0, hal3, 0);
AudioConnection          patchCord08(hal3, 0, hal4a, 0);
AudioConnection          patchCord09(hal3, 0, hal4b, 0);

AudioConnection          patchCord10(hal4a, 0, hal5a, 0);
AudioConnection          patchCord11(hal5a, 0, mixL, 1);

AudioConnection          patchCord12(hal4b, 0, hal5b, 0);
AudioConnection          patchCord13(hal5b, 0, mixR, 1);

AudioConnection          patchCord14(mixL, 0, i2s1, 0);
AudioConnection          patchCord15(mixR, 0, i2s1, 1);



// some globals
static const uint32_t DEL1LEN = 2167;
static const uint32_t DEL2LEN = 1527;
static const uint32_t DEL3LEN = 1067;
static const uint32_t DEL4aLEN = 787;
static const uint32_t DEL5aLEN = 481;
static const uint32_t DEL4bLEN = 793;
static const uint32_t DEL5bLEN = 477;

int16_t dely1buf[DEL1LEN];
int16_t dely2buf[DEL2LEN];
int16_t dely3buf[DEL3LEN];
int16_t dely4abuf[DEL4aLEN];
int16_t dely5abuf[DEL5aLEN];
int16_t dely4bbuf[DEL4bLEN];
int16_t dely5bbuf[DEL5bLEN];

static uint32_t next;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial.println("Setup");

  AudioMemory(20);

  AudioNoInterrupts();

  noisedecay.length(15);

  noisegen.amplitude(0.5);

  //                   
  hal1.setCoefficients(24576, DEL1LEN, dely1buf);
  hal2.setCoefficients(23592, DEL2LEN, dely2buf);
  hal3.setCoefficients(22642, DEL3LEN, dely3buf);
  hal4a.setCoefficients(21266, DEL4aLEN, dely4abuf);
  hal5a.setCoefficients(21692, DEL5aLEN, dely5abuf);
  hal4b.setCoefficients(21168, DEL4bLEN, dely4bbuf);
  hal5b.setCoefficients(21823, DEL5bLEN, dely5bbuf);

  mixL.gain(0, 1.0);
  mixL.gain(1, 1.0);
  mixR.gain(0, 1.0);
  mixR.gain(1, 1.0);

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

    //hal1.inspect();
    
  }

}
