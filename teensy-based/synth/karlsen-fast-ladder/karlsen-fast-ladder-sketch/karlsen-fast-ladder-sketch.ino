#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "karlsen-fast-ladder.h"


/***************/

AudioSynthWaveform   gen;
AudioSynthWaveform   lfo;
AudioFilterKarlsen   filter;
AudioEffectEnvelope  vca;


AudioOutputI2S       i2s1;
AudioControlSGTL5000 sgtl5000_1;

/***************/

AudioConnection     patchCord01(gen, 0, filter, 0);
AudioConnection     patchCord02(lfo, 0, filter, 1);
AudioConnection     patchCord03(filter, 0, vca, 0);
AudioConnection     patchCord04(vca, 0, i2s1, 0);
AudioConnection     patchCord05(vca, 0, i2s1, 1);

/***************/

uint32_t next;
int32_t paramf, paramq;
int32_t calcpeek;

int16_t q1peek, ppeek, fpeek, q2peek, rezpeek;
int32_t extra, chk;
float   flpeek, fl2peek;

/***************/

uint16_t param_update()
{
  uint16_t value;

  // ADC gives us 10 bits of data.

  value = (analogRead(A2) << 5);
  filter.q(value);
  //filter.q((value * 7) >>3);// constrain range to 7/8's of adc val to prevent freakout.
  //filter.q((value * 3) >>2);// constrain range to 3/4's of adc val to prevent freakout.
  //filter.q(0x2000);


  // To experiement with input levels.
  // I think the filter runs out of resolution and misbehaves when
  // it runs out of headroom.
  // In particular, i was seeing that manual frequency sweeps at hi resonance were causing
  // a funny signal-related DC offset to be introduced...restricting the input appears to
  // prevent that.  Apparently, attenuate by ~0.2 or more to prevent it...
  value = analogRead(A12);
  gen.amplitude((float)value / 0x3ff);

  

  // and then make up the loss with the output control.
  value = analogRead(A20);
  sgtl5000_1.volume((float)value / 0x3ff);

  value = (analogRead(A1));//was 5...1/2 the range means better LF resolution...
  filter.cutoff((value<<5) + 1); //+100);
  //filter.cutoff((value * 10.0)+20);

  return (value);
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

  lfo.frequency(.50);
  lfo.amplitude(0.5); // amplitude of 1 means 2 octaves peak-peak
  lfo.begin(WAVEFORM_SINE);
  //lfo.begin(WAVEFORM_SQUARE);
  //lfo.begin(WAVEFORM_SAWTOOTH);

  filter.octaveControl(0.0);

  vca.attack(50);
  vca.decay(250);
  vca.sustain(1.0);
  vca.release(300);

  sgtl5000_1.enable();
  sgtl5000_1.volume(0.3);

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

  if (millis() > next)
  {
    next += 1000;//125;

    switch (count % 4)
    {
      case 0:
        gen.frequency(66);
        gen.phase(0);
        vca.noteOn();

        next += 10000;
        break;

      case 1:
        vca.noteOff();
        break;

      case 2:
        gen.frequency(100);
        gen.phase(0);
        vca.noteOn();

        next += 10000;
        break;

      case 3:
        vca.noteOff();
        break;
    }

    count++;

    Serial.println(peek, HEX);

    Serial.print("Diagnostics: ");
    Serial.print(" max, buffs: ");
    Serial.print(AudioProcessorUsageMax());
    Serial.print(" ");
    Serial.println(AudioMemoryUsageMax());
    AudioProcessorUsageMaxReset();

    Serial.print("f: ");
    Serial.print(paramf, HEX);
    Serial.print(" q: ");
    Serial.print(paramq, HEX);
    Serial.print(" calc: ");
    Serial.println(calcpeek, HEX);
    
  }

}
