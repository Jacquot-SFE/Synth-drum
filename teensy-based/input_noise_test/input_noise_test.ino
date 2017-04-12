#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>


// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=200,69
AudioOutputI2S           i2s2;           //xy=365,94
AudioEffectMultiply      mult1;
AudioSynthWaveformDc     dc1;

AudioConnection          patchCord1(i2s1, 0, mult1, 0);
AudioConnection          patchCord2(dc1, 0, mult1, 1);
AudioConnection          patchCord3(mult1, 0, i2s2, 0);
AudioConnection          patchCord4(mult1, 0, i2s2, 1);

AudioControlSGTL5000     sgtl5000_1;     //xy=302,184
// GUItool: end automatically generated code

uint32_t next;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  delay(1500);

  Serial.println("Setup");

  AudioMemory(15);

  AudioNoInterrupts();

  sgtl5000_1.enable();
  sgtl5000_1.volume(0.8);


  //
  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN );
  //sgtl5000_1.inputSelect(AUDIO_INPUT_MIC );
  //sgtl5000_1.micGain(0);

  // scales are reversed: 0 allows highest input, 15 the lowest.
  // ie: 15 is noisiest
  sgtl5000_1.lineInLevel(0);

  // 13 is highest voltage, 31 the lowest.
  //sgtl5000_1.lineOutLevel(13);

  // Testing...
  sgtl5000_1.adcHighPassFilterDisable();
  //sgtl5000_1.adcHighPassFilterFreeze();
  //sgtl5000_1.audioProcessorDisable();

  //dc1.amplitude(200.0);
  dc1.amplitude(1.0);

  AudioInterrupts();

  next = millis() + 2000;

}

void loop() {
  // put your main code here, to run repeatedly:

  static bool led = false;

  if (millis() >= next)
  {
    next = millis() + 1000;

#if 1
    Serial.print("Diagnostics: max: ");
    Serial.print(" ");
    Serial.print(AudioProcessorUsageMax());
    Serial.print(" ");
    Serial.println(AudioMemoryUsageMax());
    AudioProcessorUsageMaxReset();
#endif

#if 0
    dc1.amplitude((led?0.0:1.0),250);
    led = !led;
#endif
    //um1.noteOn();
  }

}
