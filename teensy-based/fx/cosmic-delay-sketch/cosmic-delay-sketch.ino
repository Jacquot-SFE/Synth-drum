/* Cosmic-delay demo sketch.
 *  
 * A tape-delay inspired delay sketch, with heavy filtering and 
 * subtle distortion in the feedback loop.
 *  
 * Copyright (c) 2016, Byron Jacquot, SparkFun Electronics
 * SparkFun code, firmware, and software is released under 
 * the MIT License(http://opensource.org/licenses/MIT).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * 
 */


#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "mod-delay.h"
#include "cubic-distort.h"

/***************/

AudioSynthWaveform   gen;
AudioEffectEnvelope  vca;

AudioSynthNoiseWhite noise;

AudioMixer4              inmix;
AudioSynthWaveformDc     dlyctrl;
AudioEffectModDelay      xdly;
AudioFilterBiquad        delayfilt;
AudioEffectCubicDistort  fbdist;
AudioMixer4              outmix;

AudioOutputI2S       i2s1;           
AudioControlSGTL5000 sgtl5000_1;

/***************/

AudioConnection     patchCord01(gen, 0, vca, 0);
AudioConnection     patchCord02(vca, 0, inmix, 0);
AudioConnection     patchCord03(noise, 0, inmix, 2);
AudioConnection     patchCord04(inmix, 0, fbdist, 0);
AudioConnection     patchCord05(fbdist, 0, xdly, 0);
AudioConnection     patchCord06(xdly, 0, delayfilt, 0);
AudioConnection     patchCord07(delayfilt, 0, inmix, 1);

AudioConnection     patchCord100(dlyctrl,0, xdly, 1);

AudioConnection     patchCord09(vca, 0, outmix, 0);
AudioConnection     patchCord10(delayfilt, 0, outmix, 1);

AudioConnection     patchCord11(outmix, 0, i2s1, 0);
AudioConnection     patchCord12(outmix, 0, i2s1, 1);

/***************/


// This is about max (97%!) for internal RAM
static const uint32_t LEN = 0x06800;
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
  //value &= 0x3fc;
  //value |=1;
  dlyctrl.amplitude((float)value/0x3ff, 2);

  // output volume control.
  value = analogRead(A20);
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
  //inmix.gain(1, 0.9);// set by knob
  inmix.gain(2, 0.02);

  // Telephone freq response 
  // Lowpass tunung and Q have a lot to do with breakaway character and behavior!
  // If the Q gets too high, runaway gets more distorted.  0.4 or 0.5 make for
  // more aggressive breakaway, but also get crunchy.  0.2 almost doesn't break.
  delayfilt.setLowpass(0, 5000, 0.4);
  delayfilt.setHighpass(1, 120, 0.6);

  outmix.gain(0, 1.0);
  outmix.gain(1, 1.0);

  vca.attack(50);
  vca.decay(50);
  vca.sustain(0.5);
  vca.release(25);

  xdly.setbuf(LEN, delaybuf);

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

