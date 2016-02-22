#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "cosmic-delay.h"
#include "cubic-distort.h"

/***************/

AudioSynthWaveform   gen;
AudioEffectEnvelope  vca;

AudioSynthNoiseWhite noise;

AudioMixer4              inmix;
AudioSynthWaveformDc     dlyctrl;
AudioEffectCosmicDelay   xdly;
AudioFilterBiquad        aliasfilt;
AudioEffectCubicDistort  fbdist;
AudioFilterBiquad        fbfilt;
AudioMixer4              outmix;

AudioOutputI2S       i2s1;           
AudioControlSGTL5000 sgtl5000_1;

/***************/

AudioConnection     patchCord01(gen, 0, vca, 0);
AudioConnection     patchCord02(vca, 0, inmix, 0);
AudioConnection     patchCord03(noise, 0, inmix, 2);
AudioConnection     patchCord04(inmix, 0, aliasfilt, 0);
AudioConnection     patchCord05(aliasfilt, 0, fbdist, 0);
AudioConnection     patchCord06(fbdist, 0, xdly, 0);
AudioConnection     patchCord07(xdly, 0, fbfilt, 0);
AudioConnection     patchCord08(fbfilt, 0, inmix, 1);

AudioConnection     patchCord100(dlyctrl,0, xdly, 1);

AudioConnection     patchCord09(vca, 0, outmix, 0);
AudioConnection     patchCord10(xdly, 0, outmix, 1);

AudioConnection     patchCord11(outmix, 0, i2s1, 0);
AudioConnection     patchCord12(outmix, 0, i2s1, 1);

/***************/

static const uint32_t LEN = 0x06000;
int16_t delaybuf[LEN];

uint32_t next;


/***************/

void param_update()
{
  uint16_t value;

  // ADC gives us 10 bits of data.
  value = analogRead(A2);

  inmix.gain(1, (float)(value*1.3/0x3ff));

  value = analogRead(A1);
  value &= 0x3fc;
  value |=1;
  //xdly.delaylen(value<<5);
  dlyctrl.amplitude((float)value/0x3ff, 2);

  // add output volume control.
  value = analogRead(A13);
  sgtl5000_1.volume((float)value/0x3ff);


}



void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial.println("Setup");

  AudioMemory(20);

  AudioNoInterrupts();

  gen.frequency(100);
  gen.amplitude(0.10);
  gen.begin(WAVEFORM_SAWTOOTH);
  //gen.begin(WAVEFORM_SQUARE);

  // Adding in some noise can trigger spontaneous breakaway.
  //noise.amplitude(0.025);

  inmix.gain(0, 1.0);
  //inmix.gain(1, 0.9);
  inmix.gain(2, 0.02);

  // Probably not needed
  aliasfilt.setLowpass(0, 15000, 0.5);

  // Telephone freq response in feedback loop
  // Lowpass tunung and Q have a lot to do with breakaway character and behavior!
  // If the Q gets too high, runaway gets more distorted.  0.4 or 0.5 make for
  // more aggressive breakaway, but also get crunchy.  0.2 almost doesn't break.
  fbfilt.setLowpass(0, 3500, 0.4);
  fbfilt.setHighpass(1, 150, 0.6);

  outmix.gain(0, 1.0);
  outmix.gain(1, 1.0);

  vca.attack(50);
  vca.decay(50);
  vca.sustain(0.5);
  vca.release(25);

  //xdly.delay(0, 250);
  xdly.setbuf(LEN, delaybuf);
  //xdly.delaylen(LEN/2);

  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);

  AudioInterrupts();

  next = millis() + 1000;

  Serial.println("Setup complete.");

}



void loop() {
  // put your main code here, to run repeatedly:
  static bool next_on = true;

  static uint32_t count = 0;

  param_update();

  if(millis() > next)
  {
    next += 125;
#if 1
    switch(count % 4)
    {
      case 0:
        gen.frequency(66);
        gen.phase(0);
        vca.noteOn();
      break;

      case 1:
        vca.noteOff();
        next += 3000;
      break;

      case 2:
        gen.frequency(100);
        gen.phase(0);
        vca.noteOn();
      break;

      case 3:
        vca.noteOff();
        next += 3000;
      break;
    }
#endif
    count++;

    Serial.print("Diagnostics: ");
    Serial.print(" max, buffs: ");
    Serial.print(AudioProcessorUsageMax());
    Serial.print(" ");
    Serial.println(AudioMemoryUsageMax());
    AudioProcessorUsageMaxReset();
    xdly.inspect();
  }

}

