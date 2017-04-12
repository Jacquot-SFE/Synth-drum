/* RSL_ADCTest2.ino
*/
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

//Audio objects
// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=173,313
AudioMixer4              mixer1;         //xy=404,164
AudioMixer4              mixer2;         //xy=404,355
AudioOutputI2S           i2s3;           //xy=668,313
AudioConnection          patchCord2(i2s1, 0, mixer1, 0);
AudioConnection          patchCord4(i2s1, 1, mixer2, 0);
AudioConnection          patchCord5(mixer1, 0, i2s3, 0);
AudioConnection          patchCord6(mixer2, 0, i2s3, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=177,525
// GUItool: end automatically generated code

void setup() {
   AudioMemory(20);
  sgtl5000_1.enable();
  // 2 & 19 sets 2.2 v p-p range for in and out
  sgtl5000_1.lineInLevel(2);
  sgtl5000_1.lineOutLevel(19);
  mixer1.gain(0,200.0);             // To see ADC noise on scope
  mixer2.gain(0,200.0);
  }

void loop(){
  }
