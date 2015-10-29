#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

// GUItool: begin automatically generated code
AudioSynthWaveformDc     dc1;            //xy=201,443
AudioSynthWaveformDc     dc2;            //xy=255,309
AudioEffectMultiply      multiply1;      //xy=388,429
AudioMixer4              mixer1;         //xy=458,290
AudioSynthNoiseWhite     noise1;         //xy=573,86
AudioSynthWaveformSineModulated sine_fm1;       //xy=622,293
AudioEffectMultiply      multiply2;      //xy=688,450
AudioFilterStateVariable filter1;        //xy=740,136
AudioMixer4              mixer2;         //xy=853,310
AudioOutputI2S           i2s1;           //xy=968,448
AudioConnection          patchCord1(dc1, 0, multiply1, 0);
AudioConnection          patchCord2(dc1, 0, multiply1, 1);
AudioConnection          patchCord3(dc2, 0, mixer1, 1);
AudioConnection          patchCord4(multiply1, 0, multiply2, 1);
AudioConnection          patchCord5(multiply1, 0, mixer1, 0);
AudioConnection          patchCord6(multiply1, 0, filter1, 1);
AudioConnection          patchCord7(mixer1, sine_fm1);
AudioConnection          patchCord8(noise1, 0, filter1, 0);
AudioConnection          patchCord9(sine_fm1, 0, mixer2, 1);
AudioConnection          patchCord10(multiply2, 0, i2s1, 0);
AudioConnection          patchCord11(multiply2, 0, i2s1, 1);
AudioConnection          patchCord12(filter1, 0, mixer2, 0);
AudioConnection          patchCord13(mixer2, 0, multiply2, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=906,517
// GUItool: end automatically generated code


void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  pinMode(13, OUTPUT);

  AudioMemory(15);
 
  AudioNoInterrupts();
  sine_fm1.frequency(200);

  dc1.amplitude(0.0);
  dc2.amplitude(-0.95);

  sgtl5000_1.enable();
  sgtl5000_1.volume(0.75);

  noise1.amplitude(0.20);

  filter1.resonance(4.5);
  filter1.frequency(500);
  filter1.octaveControl(5);

  mixer2.gain(1,0);

  AudioInterrupts();
  
}

void loop() {
  // put your main code here, to run repeatedly:

  //AudioNoInterrupts();
  digitalWrite(13, HIGH);
  sine_fm1.phase(180);
  delay(3);
  dc1.amplitude(1.0, 1);
  //AudioInterrupts();
  delay(3);
  dc1.amplitude(0, 1200);
  digitalWrite(13, LOW);
  delay(2000);

  Serial.print("Diagnostics ");
  Serial.print(AudioProcessorUsageMax());
  Serial.print(" ");
  Serial.println(AudioMemoryUsageMax());
  AudioProcessorUsageMaxReset();

}
