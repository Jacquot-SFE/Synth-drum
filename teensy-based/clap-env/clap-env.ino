#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "Effect-ClapEnv.h"

// GUItool: begin automatically generated code
AudioSynthNoiseWhite     noise1;         //xy=573,86
//AudioSynthWaveformSineModulated sine_fm1;
AudioEffectClapEnvelope decay1;
AudioOutputI2S           i2s1;           //xy=968,448
AudioConnection          patchCord1(noise1, 0, decay1, 0);
AudioConnection          patchCord2(decay1, 0, i2s1, 0);
AudioConnection          patchCord3(decay1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=906,517
// GUItool: end automatically generated code


void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  pinMode(13, OUTPUT); // LED pin
  //pinMode(15, INPUT); // Volume pot pin?
  
  //analogReadResolution(8);

  AudioMemory(15);

  AudioNoInterrupts();
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);

  noise1.amplitude(0.5);

  //sine_fm1.frequency(220);


  AudioInterrupts();

}

void loop() {
  // put your main code here, to run repeatedly:

  uint16_t coeff;
  uint16_t adc;

  AudioNoInterrupts();
  digitalWrite(13, HIGH);
  //sine_fm1.phase(0);
  decay1.noteOn();
  AudioInterrupts();
#if 0
  Serial.println(decay1.increment);
  for (int i = 0 ; i < 50; i++)
  {
    Serial.println(decay1.current, HEX);
    Serial.println(decay1.stt, HEX);
    delay(10);
  }
#endif
  delay(1);
  digitalWrite(13, LOW);
  delay(2000);

  adc = analogRead(A1);

  Serial.print("Analog: ");
  Serial.println(adc, HEX);

  decay1.length((adc*2)+10);
  //decay1.coefficent(0xfd);

  Serial.print("Diagnostics ");
  Serial.print(AudioProcessorUsageMax());
  Serial.print(" ");
  Serial.println(AudioMemoryUsageMax());
  AudioProcessorUsageMaxReset();

}
