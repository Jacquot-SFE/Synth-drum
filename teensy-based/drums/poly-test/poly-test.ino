#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "Synth-Clatter.h"
#include "Synth-Decay.h"
#include "Synth-DrumHeart.h"
//#include "synth_simple_drum.h"

#include "panel-scanner.h"
#include "editor.h"
#include "pattern.h"
#include "player.h"

#define HAT
#define KICK
#define SNARE
#define TOM
#define SHAKER
#define BELL

// Used by multiple instruments:
AudioSynthNoiseWhite   noise;

#ifdef HAT
// hats
AudioSynthClatter      clat1;
AudioSynthDecay        hatdecay;
AudioFilterBiquad      hatfilter;
AudioEffectMultiply    hatmult;
#endif

#ifdef BELL
AudioSynthDecay        belldecay;
AudioEffectMultiply    bellmult;
#endif

#ifdef KICK
// kick
AudioSynthSimpleDrum     kick;
#endif

#ifdef SNARE
// snare
AudioSynthDrumHeart    snare;
AudioMixer4            snaremix;
AudioEffectMultiply    snaremult;
#endif

#ifdef TOM
// tom
AudioSynthSimpleDrum   tom;
#endif

#ifdef SHAKER
// shaker
AudioSynthDecay        shakedecay;
AudioFilterBiquad      shakefilter;
AudioEffectMultiply    shakemult;
#endif

// outputs
AudioMixer4            mixer1;
AudioMixer4            mixer2;

AudioOutputI2S           i2s1;           //xy=968,448

#ifdef HAT
AudioConnection          patchCord01(clat1, 0, hatfilter, 0);
AudioConnection          patchCord02(hatfilter, 0, hatmult, 0);
//AudioConnection          patchCord03(hatfilter, 0, hatmult, 0);
//AudioConnection          patchCord01(hatdecay, 0, hatmult, 0);
AudioConnection          patchCord03(hatdecay, 0, hatmult, 1);
AudioConnection          patchCord90(hatmult, 0, mixer1, 0);
#endif

#ifdef BELL
AudioConnection          patchCord04(clat1, 1, bellmult, 0);
AudioConnection          patchCord05(belldecay, 0, bellmult, 1);
AudioConnection          patchCord96(bellmult, 0,  mixer2, 2);
#endif

#ifdef KICK
AudioConnection          patchCord10(kick, 0, mixer1, 1);
#endif

#ifdef SNARE
AudioConnection          patchCord20(noise, 0, snaremix, 0);
AudioConnection          patchCord21(snare, 0, snaremix, 1);
AudioConnection          patchCord22(snare, 1, snaremult, 1);
AudioConnection          patchCord23(snaremix, 0, snaremult, 0);
AudioConnection          patchCord92(snaremult, 0, mixer1, 2);
#endif

#ifdef TOM
AudioConnection          patchCord30(tom, 0, mixer2, 0);
#endif

#ifdef SHAKER
AudioConnection          patchCord40(noise, 0, shakefilter, 0);
AudioConnection          patchCord41(shakefilter, 0, shakemult, 0);
AudioConnection          patchCord42(shakedecay, 0, shakemult, 1);
AudioConnection          patchCord95(shakemult, 0, mixer2, 1);
#endif

//AudioConnection          patchCord96(noise, 0, mixer2, 2);

AudioConnection          patchCord93(mixer2, 0, mixer1, 3);

AudioConnection          patchCord98(mixer1, 0, i2s1, 0);
AudioConnection          patchCord99(mixer1, 0, i2s1, 1);

AudioControlSGTL5000     sgtl5000_1;



// Appp construct singletons
PanelScanner theScanner;
Editor       theEditor;
Pattern      thePattern; // TBD: multidimensional...
Player       thePlayer;

// Globals for params on shared voices
uint16_t openlen, closedlen;
uint16_t t1, t2, t3;

#if 0
void trigger()
{
  static uint32_t index = 0;
  uint8_t step;

  step = thePattern.getStepData(index);// sequence[index];

  //theScanner.setLED(index);
  //theScanner.doTransaction();

#if 0
  Serial.print("Trigger: step#");
  Serial.print(index);
  Serial.print(" bitmap:");
  Serial.println(step, HEX);
#endif

  AudioNoInterrupts();

#ifdef KICK
  if (step & 0x01)
    kick.noteOn();
#endif    
#ifdef SNARE
  if (step & 0x02)
    snare.noteOn();
#endif
#ifdef HAT
  if (step & 0x04)
  {
    hatdecay.length(closedlen);
    hatdecay.noteOn();
  }
  else if (step & 0x08)
  {
    hatdecay.length(openlen);
    hatdecay.noteOn();
  }
#endif
#ifdef TOM  
  if (step & 0x10)
  {
    tom.frequency(t1);
    tom.noteOn();
  }
  else if (step & 0x20)
  {
    tom.frequency(t2);
    tom.noteOn();
  }
  else if (step & 0x40)
  {
    tom.frequency(t3);
    tom.noteOn();
  }
#endif
#ifdef SHAKER  
  if (step & 0x80)
  {
    //shakedecay.noteOn();
    belldecay.noteOn();
  }
#endif
  AudioInterrupts();
  theScanner.clearLED(index);
  index++;
  index &= 0x0f;
}
#endif

void paramInit()
{
  // common to severl instruments
  noise.amplitude(0.5);
  
#ifdef HAT
  // hat stuff
  //clat1; - no params
  //hatdecay.length(150);
  openlen = 300;
  closedlen = 50;
  hatfilter.setHighpass(0, 700, 0.2);
  hatfilter.setLowpass(1, 10000, 0.8);

  //hatdecay.frequency(1500);
  //hatdecay.length(50);
#endif

#ifdef BELL
  belldecay.length(333);
#endif

#ifdef KICK
  // kick
  kick.frequency(60);
  kick.length(100);
  kick.pitchMod(0x2f0); // 0x200 is no mod...
#endif

#ifdef SNARE
  // snare
  snare.frequency(200);
  snare.length(100);
  snare.second(true);
  snare.pitchMod(0x280);
  snaremix.gain(0, 0.75);
  snaremix.gain(1, 0.5);
#endif

#ifdef TOM
  // tom
  //tom.frequency(80);
  t1 = 60;
  t2 = 90;
  t3 = 135;
  tom.secondMix(1.0);
  tom.length(250);
  tom.pitchMod(0.75);
#endif

#ifdef SHAKER  
  // shaker
  shakefilter.setLowpass(0, 3500, 0.7);
  shakefilter.setHighpass(1, 400, 0.3);
  shakedecay.length(50);
#endif
  // Master
  mixer1.gain(0, 0.75);// hat
  mixer1.gain(1, 0.75);// kik
  mixer1.gain(2, 0.75);// snr
  mixer1.gain(3, 0.75);// mix2
  mixer2.gain(0, 0.75);// tom
  mixer2.gain(1, 0.5);//shaker
  mixer2.gain(2, 0.5);// bell
  mixer2.gain(3, 0.0);
}

void paramUpdate1()
{
  uint16_t   pitch, mod, len;
  uint16_t   snpitch, snmix, snlen;
  uint16_t   ohdec, chdec;

  pitch = analogRead(A1);
  mod = analogRead(A12);
  len = analogRead(A13);

  snpitch = analogRead(A2);
  snmix = analogRead(A3);
  snlen = analogRead(A14);

  ohdec = analogRead(A6);
  chdec = analogRead(A16);

#ifdef KICK
  kick.frequency(30 + (pitch >> 3));
  kick.pitchMod((float)mod/0x3ff);
  kick.length(len + 10);
#endif
#ifdef SNARE
  float mix = (float)snmix / 1024.0;
  snare.frequency(80 + (snpitch >> 2));
  snaremix.gain(0, 1.0 - mix);
  snaremix.gain(1, mix);
  snare.length(snlen + 10);
#endif
#ifdef HAT
  openlen = ohdec + 10;
  closedlen = chdec + 10;
#endif  
}
#if 1

void paramUpdate2()
{
  uint16_t   p1, p2, p3;
  uint16_t  len, mod;
  uint16_t  secondskin;
  uint16_t  slen;

  p1 = analogRead(A7);
  p2 = analogRead(A10);
  p3 = analogRead(A11);

  len = analogRead(A17);
  mod = analogRead(A18);
  secondskin = analogRead(A19);

  //slen = analogRead(A11);

  t1 = 30 + (p1 >> 1);
  t2 = 30 + (p2 >> 1);
  t3 = 30 + (p3 >> 1);

  tom.length(len + 10);
  tom.pitchMod((float)mod/0x3ff);
  tom.secondMix((float)secondskin/0x3ff);

  //shakedecay.length(slen + 10);
}
#endif

//static uint32_t next;

void paramUpdate3()
{
  uint16_t volume;
  uint32_t tempo;

  volume = analogRead(A20);

  sgtl5000_1.volume(((float)volume)/0x3ff);

  tempo = analogRead(A15);

  tempo = 0x3ff - tempo;

  tempo *= 225;
  tempo >>= 10;
  tempo &= 0x3ff;

  //pause = tempo + 75;
  thePlayer.setPause(tempo+75);
}




void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
 
  delay(500);

  Serial.println("Setup");

  pinMode(15, INPUT); // Volume pot pin?

  theScanner.initScanning();
  //theScanner.doTransaction();

  theEditor.setMode(Editor::eMODE_STEP_EDIT);

  // audio library init
  AudioMemory(20);

  //next = millis() + 1000;

  // read panel before we start to run
  //paramUpdate();

  AudioNoInterrupts();

  //decay1.length(1000);

  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);

  paramInit();

  AudioInterrupts();

  delay(500);

  Serial.println("Setup Complete");
}

void loop() 
{
  uint32_t now = millis();
  static uint32_t then;
  
#if 1
  // put your main code here, to run repeatedly:

  paramUpdate1();// kik,snr,hat
  paramUpdate2();// toms, shaker
  paramUpdate3();// master volume & tempo

  if(now > then)
  {
    thePlayer.tick();
  }

  if(now % 5 == 0)
  {
    theScanner.tick();
  }

  if(now % 5000 == 0)
  {
    //theScanner.dumpLEDs();

    
    Serial.print("Diagnostics: ");
    Serial.print(" max, buffs: ");
    Serial.print(AudioProcessorUsageMax());
    Serial.print(" ");
    Serial.println(AudioMemoryUsageMax());
    AudioProcessorUsageMaxReset();
  }

  then = now;
#endif  
}
