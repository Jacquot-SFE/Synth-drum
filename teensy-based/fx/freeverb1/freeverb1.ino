#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "filter_lbcf.h"
#include "filter_ap.h"

// GUItool: begin automatically generated code
//AudioPlaySdWav           playWav1;       //xy=139,343
AudioInputI2S           playWav1;
AudioOutputI2S           i2s1;           //xy=1121,347
AudioFilterLBCF           lbcf1;           //xy=342,521
AudioFilterLBCF           lbcf2;           //xy=344,463
AudioFilterLBCF           lbcf3;           //xy=346,585
AudioFilterLBCF           lbcf4;           //xy=349,403
AudioFilterLBCF           lbcf5;           //xy=353,147
AudioFilterLBCF           lbcf6;           //xy=353,211
AudioFilterLBCF           lbcf7;           //xy=356,278
AudioFilterLBCF           lbcf8;           //xy=359,338

AudioMixer4              mixer1;         //xy=563,255
AudioMixer4              mixer2;         //xy=568,478
AudioMixer4              mixer3;         //xy=733,348

AudioFilterAP        ap1;        //xy=903,345

AudioConnection          patchCord1(playWav1, 0, lbcf1, 0);
AudioConnection          patchCord2(playWav1, 0, lbcf2, 0);
AudioConnection          patchCord3(playWav1, 0, lbcf3, 0);
AudioConnection          patchCord4(playWav1, 0, lbcf4, 0);
AudioConnection          patchCord5(playWav1, 0, lbcf5, 0);
AudioConnection          patchCord6(playWav1, 0, lbcf6, 0);
AudioConnection          patchCord7(playWav1, 0, lbcf7, 0);
AudioConnection          patchCord8(playWav1, 0, lbcf8, 0);

AudioConnection          patchCord13(lbcf1, 0, mixer1, 0);
AudioConnection          patchCord14(lbcf2, 0, mixer1, 1);
AudioConnection          patchCord15(lbcf3, 0, mixer1, 2);
AudioConnection          patchCord16(lbcf4, 0, mixer1, 3);
AudioConnection          patchCord9(lbcf7, 0, mixer2, 2);
AudioConnection          patchCord10(lbcf6, 0, mixer2, 1);
AudioConnection          patchCord11(lbcf8, 0, mixer2, 3);
AudioConnection          patchCord12(lbcf5, 0, mixer2, 0);

AudioConnection          patchCord17(mixer1, 0, mixer3, 0);
AudioConnection          patchCord18(mixer2, 0, mixer3, 1);

AudioConnection          patchCord19(mixer3, ap1);


AudioConnection          patchCord20(ap1, 0, i2s1, 0);
AudioConnection          patchCord21(ap1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=575,703
// GUItool: end automatically generated code

#define LBCF_DELAY_LENGTH1 (1557)
#define LBCF_DELAY_LENGTH2 (1617)
#define LBCF_DELAY_LENGTH3 (1491)
#define LBCF_DELAY_LENGTH4 (1422)
#define LBCF_DELAY_LENGTH5 (1277)
#define LBCF_DELAY_LENGTH6 (1356)
#define LBCF_DELAY_LENGTH7 (1188)
#define LBCF_DELAY_LENGTH8 (1116)

#define AP_DELAY_LENGTH1 (225)
#define AP_DELAY_LENGTH2 (556)
#define AP_DELAY_LENGTH3 (441)
#define AP_DELAY_LENGTH4 (341)

uint32_t delayline1[LBCF_DELAY_LENGTH1/2];
uint32_t delayline2[LBCF_DELAY_LENGTH2/2];
uint32_t delayline3[LBCF_DELAY_LENGTH3/2];
uint32_t delayline4[LBCF_DELAY_LENGTH4/2];
uint32_t delayline5[LBCF_DELAY_LENGTH5/2];
uint32_t delayline6[LBCF_DELAY_LENGTH6/2];
uint32_t delayline7[LBCF_DELAY_LENGTH7/2];
uint32_t delayline8[LBCF_DELAY_LENGTH8/2];

uint32_t ap_in_delayline1[ AP_DELAY_LENGTH1/2];
uint32_t ap_in_delayline2[ AP_DELAY_LENGTH2/2];
uint32_t ap_in_delayline3[ AP_DELAY_LENGTH3/2];
uint32_t ap_in_delayline4[ AP_DELAY_LENGTH4/2];
uint32_t ap_out_delayline1[ AP_DELAY_LENGTH1/2];
uint32_t ap_out_delayline2[ AP_DELAY_LENGTH2/2];
uint32_t ap_out_delayline3[ AP_DELAY_LENGTH3/2];
uint32_t ap_out_delayline4[ AP_DELAY_LENGTH4/2];

const double coeffs[2] = {0.84, 0.2};
const double g = 0.5;

static uint32_t next;

void setup() {

  Serial.begin(115200);

  delay(1000);
  
  Serial.print("Starting init...");
  AudioMemory(50);

  AudioNoInterrupts();
  
  sgtl5000_1.enable();
  
  sgtl5000_1.volume(0.75);

  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
  sgtl5000_1.lineInLevel(1);

  for (int i=0; i < LBCF_DELAY_LENGTH1/2; i++)
      delayline1[i] =0;
   for (int i=0; i < LBCF_DELAY_LENGTH2/2; i++)
      delayline2[i] =0;
   for (int i=0; i < LBCF_DELAY_LENGTH3/2; i++)
      delayline3[i] =0;
   for (int i=0; i < LBCF_DELAY_LENGTH4/2; i++)
      delayline4[i] =0;
   for (int i=0; i < LBCF_DELAY_LENGTH5/2; i++)
      delayline5[i] =0;
   for (int i=0; i < LBCF_DELAY_LENGTH6/2; i++)
      delayline6[i] =0;
   for (int i=0; i < LBCF_DELAY_LENGTH7/2; i++)
      delayline7[i] =0;
   for (int i=0; i < LBCF_DELAY_LENGTH8/2; i++)
      delayline8[i] =0;

    for (int i=0; i < AP_DELAY_LENGTH1/2; i++) {
      ap_in_delayline1[i] =0;  
      ap_out_delayline1[i]=0;
    }
    for (int i=0; i < AP_DELAY_LENGTH2/2; i++) {
      ap_in_delayline2[i] =0;  
      ap_out_delayline2[i]=0;
    }
    for (int i=0; i < AP_DELAY_LENGTH3/2; i++) {
      ap_in_delayline3[i] =0;  
      ap_out_delayline3[i]=0;
    }
    for (int i=0; i < AP_DELAY_LENGTH4/2; i++) {
      ap_in_delayline4[i] =0;  
      ap_out_delayline4[i]=0;
    }


  //SPI.setMOSI(7);
  //SPI.setSCK(14);
  //if (SD.begin(10)) {
  //  playWav1.play("SDTEST1.WAV");
  // }
  lbcf1.setCoefficients(0, coeffs, delayline1, LBCF_DELAY_LENGTH1/2);
  lbcf2.setCoefficients(0, coeffs, delayline2, LBCF_DELAY_LENGTH2/2);
  lbcf3.setCoefficients(0, coeffs, delayline3, LBCF_DELAY_LENGTH3/2);
  lbcf4.setCoefficients(0, coeffs, delayline4, LBCF_DELAY_LENGTH4/2);
  lbcf5.setCoefficients(0, coeffs, delayline5, LBCF_DELAY_LENGTH5/2);
  lbcf6.setCoefficients(0, coeffs, delayline6, LBCF_DELAY_LENGTH6/2);
  lbcf7.setCoefficients(0, coeffs, delayline7, LBCF_DELAY_LENGTH7/2);
  lbcf8.setCoefficients(0, coeffs, delayline8, LBCF_DELAY_LENGTH8/2);

  mixer1.gain(0, 0.25);
  mixer1.gain(1, 0.25);
  mixer1.gain(2, 0.25);
  mixer1.gain(3, 0.25);
  mixer2.gain(0, 0.25);
  mixer2.gain(1, 0.25);
  mixer2.gain(2, 0.25);
  mixer2.gain(3, 0.25);
  
  mixer3.gain(0,0.5);
  mixer3.gain(1,0.5);

  ap1.setCoefficients(0, g, ap_in_delayline1, ap_out_delayline1, AP_DELAY_LENGTH1/2);
  ap1.setCoefficients(1, g, ap_in_delayline2, ap_out_delayline2, AP_DELAY_LENGTH2/2);
  ap1.setCoefficients(2, g, ap_in_delayline3, ap_out_delayline3, AP_DELAY_LENGTH3/2);
  ap1.setCoefficients(3, g, ap_in_delayline4, ap_out_delayline4, AP_DELAY_LENGTH4/2);
  

  next = millis() + 5000;

  AudioInterrupts();

  Serial.println("Init complete.");
}

void loop() 
{
  if(millis() == next)
  {
    next = millis() + 5000;

    Serial.print("Diagnostics ");
    Serial.print(AudioProcessorUsageMax());
    Serial.print(" ");
    Serial.println(AudioMemoryUsageMax());
    AudioProcessorUsageMaxReset();
  }
}

