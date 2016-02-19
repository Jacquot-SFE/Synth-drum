#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "cosmic-delay.h"

/***************/

AudioSynthWaveform   gen;
AudioEffectEnvelope  vca;

AudioSynthNoiseWhite noise;

AudioMixer4              inmix;
AudioEffectCosmicDelay   xdly;
AudioFilterBiquad        aliasfilt;
AudioFilterBiquad        fbfilt;
AudioMixer4              outmix;

AudioOutputI2S       i2s1;           
AudioControlSGTL5000 sgtl5000_1;

/***************/

AudioConnection     patchCord01(gen, 0, vca, 0);
AudioConnection     patchCord02(vca, 0, inmix, 0);
AudioConnection     patchCord03(noise, 0, inmix, 2);
AudioConnection     patchCord04(inmix, 0, aliasfilt, 0);
AudioConnection     patchCord05(aliasfilt, 0, xdly, 0);
AudioConnection     patchCord06(xdly, 0, fbfilt, 0);
AudioConnection     patchCord07(fbfilt, 0, inmix, 1);

AudioConnection     patchCord08(vca, 0, outmix, 0);
AudioConnection     patchCord09(xdly, 0, outmix, 1);

AudioConnection     patchCord10(vca, 0, i2s1, 0);
AudioConnection     patchCord11(outmix, 0, i2s1, 1);

/***************/

static const uint32_t LEN = 0x04000;
int16_t delaybuf[LEN];

uint32_t next;


/***************/

void param_update()
{
  uint16_t value;

  // ADC gives us 10 bits of data.
  value = analogRead(A2);

  inmix.gain(1, (float)(value*1.1/0x3ff));

  value = analogRead(A1);
  xdly.delaylen(value<<5);

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

  //noise.amplitude(0.2);

  inmix.gain(0, 1.0);
  //inmix.gain(1, 0.9);
  inmix.gain(2, 0.02);

  aliasfilt.setLowpass(0, 10000, 0.7);

  fbfilt.setLowpass(0, 2000, 0.7);
  fbfilt.setHighpass(1, 150, 0.7);

  outmix.gain(0, 0.0);// WAS 1.0..
  outmix.gain(1, 1.0);

  vca.attack(50);
  vca.decay(250);
  vca.sustain(0.5);
  vca.release(25);

  //xdly.delay(0, 250);
  xdly.setbuf(LEN, delaybuf);
  xdly.delaylen(LEN/2);

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

    switch(count % 4)
    {
      case 0:
        gen.frequency(66);
        gen.phase(0);
        vca.noteOn();
      break;

      case 1:
        vca.noteOff();
        next += 1000;
      break;

      case 2:
        gen.frequency(100);
        gen.phase(0);
        vca.noteOn();
      break;

      case 3:
        vca.noteOff();
        next += 1000;
      break;
    }

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

