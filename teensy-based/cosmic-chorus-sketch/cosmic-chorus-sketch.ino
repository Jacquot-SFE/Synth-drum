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

AudioSynthWaveformDc     dlybiasl;
AudioSynthWaveformSine   lfo;
AudioSynthWaveformDc     invertbias;
AudioEffectMultiply      invert;
AudioEffectCosmicDelay   xdlyl;
AudioEffectCosmicDelay   xdlyr;
AudioMixer4              modmixl;
AudioMixer4              modmixr;
AudioFilterBiquad        aliasfilt;
AudioEffectCubicDistort  fbdist;
AudioFilterBiquad        fbfilt;
AudioMixer4              outmixl;
AudioMixer4              outmixr;

AudioOutputI2S       i2s1;
AudioControlSGTL5000 sgtl5000_1;

/***************/

AudioConnection     patchCord01(gen, 0, vca, 0);

AudioConnection     patchCord02(vca, 0, xdlyl, 0);
AudioConnection     patchCord03(vca, 0, xdlyr, 0);



AudioConnection     patchCord04(dlybiasl, 0, modmixl, 0);
AudioConnection     patchCord05(lfo, 0, modmixl, 1);
AudioConnection     patchCord06(modmixl, 0, xdlyl, 1);

AudioConnection     patchCord07(dlybiasl, 0, modmixr, 0);
AudioConnection     patchCord08(invertbias, 0, invert, 0);
AudioConnection     patchCord09(lfo, 0, invert, 1);
AudioConnection     patchCord10(invert, 0, modmixr, 1);
AudioConnection     patchCord11(modmixr, 0, xdlyr, 1);


AudioConnection     patchCord12(vca, 0, outmixl, 0);
AudioConnection     patchCord13(xdlyl, 0, outmixl, 1);

AudioConnection     patchCord14(vca, 0, outmixr, 0);
AudioConnection     patchCord15(xdlyr, 0, outmixr, 1);

AudioConnection     patchCord16(outmixl, 0, i2s1, 0);
AudioConnection     patchCord17(outmixr, 0, i2s1, 1);

/***************/

static const uint32_t LEN = 0x0800;
int16_t delaybufl[LEN];
int16_t delaybufr[LEN];

uint32_t next;


/***************/

void param_update()
{
  uint16_t value;

  // ADC gives us 10 bits of data.
  value = analogRead(A1);
  lfo.frequency((float)(value * 10.0 / 0x3ff));

  value = analogRead(A2);
  lfo.amplitude((float)(value * 1.0 / 0x3ff));

  value = analogRead(A12);
  outmixl.gain(1, (float)(value * 1.0 / 0x3ff));
  outmixr.gain(1, (float)(value * 1.0 / 0x3ff));

#if 0
  value = analogRead(A1);
  value &= 0x3fc;
  value |= 1;
  //xdly.delaylen(value<<5);
  dlyctrl.amplitude((float)value / 0x3ff, 2);
#endif

  // add output volume control.
  value = analogRead(A13);
  sgtl5000_1.volume((float)value / 0x3ff);


}



void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial.println("Setup");

  AudioMemory(30);

  AudioNoInterrupts();

  gen.frequency(100);
  gen.amplitude(0.10);
  //gen.begin(WAVEFORM_SAWTOOTH);
  //gen.begin(WAVEFORM_SQUARE);
  gen.begin(WAVEFORM_TRIANGLE);


#if 0
  // Telephone freq response in feedback loop
  // Lowpass tunung and Q have a lot to do with breakaway character and behavior!
  // If the Q gets too high, runaway gets more distorted.  0.4 or 0.5 make for
  // more aggressive breakaway, but also get crunchy.  0.2 almost doesn't break.
  fbfilt.setLowpass(0, 3500, 0.4);
  fbfilt.setHighpass(1, 150, 0.6);
#endif

  lfo.frequency(.1);
  lfo.amplitude(0.10);
  dlybiasl.amplitude(0.5);

  invertbias.amplitude(-1.0);

  modmixl.gain(0, 1.0);
  modmixl.gain(1, 0.5);

  modmixr.gain(0, 1.0);
  modmixr.gain(1, 0.5);

  outmixl.gain(0, 1.0);
  outmixl.gain(1, 1.0);
  outmixr.gain(0, 1.0);
  outmixr.gain(1, 1.0);

  vca.attack(50);
  vca.decay(50);
  vca.sustain(0.5);
  vca.release(200);

  xdlyl.setbuf(LEN, delaybufl);
  xdlyr.setbuf(LEN, delaybufr);

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

  if (millis() > next)
  {
    next += 500;
#if 1
    switch (count % 4)
    {
      case 0:
        gen.frequency(66);
        gen.phase(0);
        vca.noteOn();
        next += 1500;
        break;

      case 1:
        vca.noteOff();
        //next += 3000;
        break;

      case 2:
        gen.frequency(100);
        gen.phase(0);
        vca.noteOn();
        next += 1500;
        break;

      case 3:
        vca.noteOff();
        //next += 3000;
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
    xdlyr.inspect();
  }

}

