#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "karlsen-fast-ladder.h"
#include "Effect-ExponentialDecay.h"


/***************/

AudioSynthWaveform   gen;
AudioSynthWaveformDc  dc1;
AudioSynthWaveformDc  dc;

AudioEffectExponentialDecay envgen;
AudioEffectMultiply  mul;
AudioMixer4          mixer;
AudioFilterKarlsen   filter;


AudioOutputI2S       i2s1;
AudioControlSGTL5000 sgtl5000_1;

/***************/

AudioConnection     patchCord01(gen, 0, filter, 0);
AudioConnection     patchCord02(envgen,0, mixer, 0);
AudioConnection     patchCord03(dc, 0, mixer, 1);
AudioConnection     patchCord04(mixer, 0, filter, 1);
AudioConnection     patchCord05(filter, 0, mul, 0);
AudioConnection     patchCord66(dc1, 0, envgen, 0);
AudioConnection     patchCord06(envgen, 0, mul, 1);
AudioConnection     patchCord07(mul, 0, i2s1, 0);
AudioConnection     patchCord08(mul, 0, i2s1, 1);

/***************/

uint32_t next;
uint32_t step;

/***************/

static bool triggers[16] = {true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true };
static float pitches[16] = {50.0, 50.0, 75.0, 75.0, 50.0, 50.0, 75.0, 100.0, 50.0, 50.0, 75.0, 75.0, 100.0, 100.0, 75.0, 50.0 };

uint16_t param_update()
{
  uint16_t value;

  // ADC gives us 10 bits of data.

  value = (analogRead(A2) << 5);
  filter.q(value);

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

  // lin cutoff
  value = (analogRead(A1));//was 5...1/2 the range means better LF resolution...
  filter.cutoff((value<<5) + 1); //+100);

  // exp cutoff
  value = analogRead(A3);
  dc.amplitude((float)value / 0x3ff,2);

  // decay->cutoff modulation
  value = analogRead(A7);
  mixer.gain(0, (float)value/ 0x3ff);

  value = analogRead(A6);
  envgen.length(value );

#if 1
  value = analogRead(A13);
  if(value > 0x200)
      gen.begin(WAVEFORM_SAWTOOTH);
  else
      gen.begin(WAVEFORM_SQUARE);
#endif
  
  return (value);
}

void seqAdvance()
{
  step++;
  step &= 0x0f;

  if(triggers[step])
  {
    AudioNoInterrupts();

    gen.frequency(pitches[step]);
    gen.phase(0);
    envgen.noteOn();

    AudioInterrupts();
  }
  
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


  dc1.amplitude(1.0);

#if 1
  filter.octaveControl(7.0);

  mixer.gain(0, 0);//env depth
  mixer.gain(1, 1.0);// knob
#endif


#if 1
  envgen.length(200);
#else
  vca.attack(50);
  vca.decay(250);
  vca.sustain(1.0);
  vca.release(300);
#endif
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
    next += 150;

    seqAdvance();

    count++;

    //Serial.println(peek, HEX);

    Serial.println(envgen.current, HEX);
    Serial.println(envgen.increment, HEX);

    Serial.print("Diagnostics: ");
    Serial.print(" max, buffs: ");
    Serial.print(AudioProcessorUsageMax());
    Serial.print(" ");
    Serial.println(AudioMemoryUsageMax());
    AudioProcessorUsageMaxReset();

  }

}
