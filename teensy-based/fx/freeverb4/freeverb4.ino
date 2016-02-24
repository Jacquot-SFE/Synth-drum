#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include "filter_lbcf.h"
#include "filter_ap.h"
// GUItool: begin automatically generated code
//AudioPlaySdWav           playWav1;       //xy=61,369
AudioInputI2S           playWav1;
AudioMixer4              mixer6;         //xy=211,373
AudioFilterLBCF        lbcf8;        //xy=424,340
AudioFilterLBCF        lbcf7;        //xy=427,300
AudioFilterLBCF        lbcf5;        //xy=428,220
AudioFilterLBCF        lbcf6;        //xy=428,261
AudioFilterLBCF        lbcf4;        //xy=429,175
AudioFilterLBCF        lbcf9;        //xy=429,415
AudioFilterLBCF        lbcf1;        //xy=431,49
AudioFilterLBCF        lbcf3;        //xy=432,132
AudioFilterLBCF        lbcf10;       //xy=431,459
AudioFilterLBCF        lbcf11;       //xy=431,513
AudioFilterLBCF        lbcf12;       //xy=432,558
AudioFilterLBCF        lbcf2;        //xy=435,91
AudioFilterLBCF        lbcf13;       //xy=433,606
AudioFilterLBCF        lbcf14;       //xy=433,653
AudioFilterLBCF        lbcf15;       //xy=434,700
AudioFilterLBCF        lbcf16;       //xy=434,749
AudioMixer4              mixer1;         //xy=695,249
AudioMixer4              mixer2;         //xy=696,369
AudioMixer4              mixer4;         //xy=701,477
AudioMixer4              mixer5;         //xy=709,591
AudioMixer4              mixer3;         //xy=874,299
AudioMixer4              mixer8;         //xy=883,501
AudioFilterAP           ap2;           //xy=1044,501
AudioFilterAP           ap1;           //xy=1052,299
AudioMixer4              mixer7;         //xy=1206,356
AudioOutputI2S           i2s1;           //xy=1343,355
AudioConnection          patchCord1(playWav1, 0, mixer6, 0);
AudioConnection          patchCord2(playWav1, 0, mixer7, 1);
AudioConnection          patchCord3(playWav1, 1, mixer6, 1);
AudioConnection          patchCord4(playWav1, 1, mixer7, 3);
AudioConnection          patchCord5(mixer6, lbcf1);
AudioConnection          patchCord6(mixer6, lbcf2);
AudioConnection          patchCord7(mixer6, lbcf3);
AudioConnection          patchCord8(mixer6, lbcf4);
AudioConnection          patchCord9(mixer6, lbcf5);
AudioConnection          patchCord10(mixer6, lbcf6);
AudioConnection          patchCord11(mixer6, lbcf7);
AudioConnection          patchCord12(mixer6, lbcf8);
AudioConnection          patchCord13(mixer6, lbcf9);
AudioConnection          patchCord14(mixer6, lbcf10);
AudioConnection          patchCord15(mixer6, lbcf11);
AudioConnection          patchCord16(mixer6, lbcf12);
AudioConnection          patchCord17(mixer6, lbcf13);
AudioConnection          patchCord18(mixer6, lbcf14);
AudioConnection          patchCord19(mixer6, lbcf15);
AudioConnection          patchCord20(mixer6, lbcf16);
AudioConnection          patchCord21(lbcf8, 0, mixer2, 3);
AudioConnection          patchCord22(lbcf7, 0, mixer2, 2);
AudioConnection          patchCord23(lbcf5, 0, mixer2, 0);
AudioConnection          patchCord24(lbcf6, 0, mixer2, 1);
AudioConnection          patchCord25(lbcf4, 0, mixer1, 3);
AudioConnection          patchCord26(lbcf9, 0, mixer4, 0);
AudioConnection          patchCord27(lbcf1, 0, mixer1, 0);
AudioConnection          patchCord28(lbcf3, 0, mixer1, 2);
AudioConnection          patchCord29(lbcf10, 0, mixer4, 1);
AudioConnection          patchCord30(lbcf11, 0, mixer4, 2);
AudioConnection          patchCord31(lbcf12, 0, mixer4, 3);
AudioConnection          patchCord32(lbcf2, 0, mixer1, 1);
AudioConnection          patchCord33(lbcf13, 0, mixer5, 0);
AudioConnection          patchCord34(lbcf14, 0, mixer5, 1);
AudioConnection          patchCord35(lbcf15, 0, mixer5, 2);
AudioConnection          patchCord36(lbcf16, 0, mixer5, 3);
AudioConnection          patchCord37(mixer1, 0, mixer3, 0);
AudioConnection          patchCord38(mixer2, 0, mixer3, 1);
AudioConnection          patchCord39(mixer4, 0, mixer8, 0);
AudioConnection          patchCord40(mixer5, 0, mixer8, 1);
AudioConnection          patchCord41(mixer3, ap1);
AudioConnection          patchCord42(mixer8, ap2);
AudioConnection          patchCord43(ap2, 0, mixer7, 2);
AudioConnection          patchCord44(ap1, 0, mixer7, 0);

AudioConnection          patchCord45(mixer7, 0, i2s1, 0);
AudioConnection          patchCord46(mixer7, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=1243,666
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
uint32_t delayline9[LBCF_DELAY_LENGTH1/2];
uint32_t delayline10[LBCF_DELAY_LENGTH2/2];
uint32_t delayline11[LBCF_DELAY_LENGTH3/2];
uint32_t delayline12[LBCF_DELAY_LENGTH4/2];
uint32_t delayline13[LBCF_DELAY_LENGTH5/2];
uint32_t delayline14[LBCF_DELAY_LENGTH6/2];
uint32_t delayline15[LBCF_DELAY_LENGTH7/2];
uint32_t delayline16[LBCF_DELAY_LENGTH8/2];

uint32_t ap1_in_delayline1[ AP_DELAY_LENGTH1/2];
uint32_t ap1_in_delayline2[ AP_DELAY_LENGTH2/2];
uint32_t ap1_in_delayline3[ AP_DELAY_LENGTH3/2];
uint32_t ap1_in_delayline4[ AP_DELAY_LENGTH4/2];
uint32_t ap1_out_delayline1[ AP_DELAY_LENGTH1/2];
uint32_t ap1_out_delayline2[ AP_DELAY_LENGTH2/2];
uint32_t ap1_out_delayline3[ AP_DELAY_LENGTH3/2];
uint32_t ap1_out_delayline4[ AP_DELAY_LENGTH4/2];
uint32_t ap2_in_delayline1[ AP_DELAY_LENGTH1/2];
uint32_t ap2_in_delayline2[ AP_DELAY_LENGTH2/2];
uint32_t ap2_in_delayline3[ AP_DELAY_LENGTH3/2];
uint32_t ap2_in_delayline4[ AP_DELAY_LENGTH4/2];
uint32_t ap2_out_delayline1[ AP_DELAY_LENGTH1/2];
uint32_t ap2_out_delayline2[ AP_DELAY_LENGTH2/2];
uint32_t ap2_out_delayline3[ AP_DELAY_LENGTH3/2];
uint32_t ap2_out_delayline4[ AP_DELAY_LENGTH4/2];

const double coeffs[2] = {0.84, 0.2};
const double g = 0.5;

static uint32_t next;


#if 0
// BSS should be zeroed at init?
void cleanBuffers()
{
  uint32_t i;

  for(i = 0; i < LBCF_DELAY_LENGTH1/2; i++)
  {
        delayline1[i] = delayline9[i] = 0;
        //delayline9[i] = 0;
  }
  for(i = 0; i < LBCF_DELAY_LENGTH2/2; i++)
  {
        delayline2[i] = 0;
        delayline10[i] = 0;
  }
  for(i = 0; i < LBCF_DELAY_LENGTH3/2; i++)
  {
        delayline3[i] = 0;
        delayline11[i] = 0;
  }
  for(i = 0; i < LBCF_DELAY_LENGTH4/2; i++)
  {
        delayline4[i] = 0;
        delayline12[i] = 0;
  }
  for(i = 0; i < LBCF_DELAY_LENGTH5/2; i++)
  {
        delayline5[i] = 0;
        delayline13[i] = 0;
  }
  for(i = 0; i < LBCF_DELAY_LENGTH6/2; i++)
  {
        delayline6[i] = 0;
        delayline14[i] = 0;
  }
  for(i = 0; i < LBCF_DELAY_LENGTH7/2; i++)
  {
        delayline7[i] = 0;
        delayline15[i] = 0;
  }
  for(i = 0; i < LBCF_DELAY_LENGTH8/2; i++)
  {
        delayline8[i] = 0;
        delayline16[i] = 0;
  }

  for(i = 0; i < AP_DELAY_LENGTH1/2; i++)
  {
    ap1_in_delayline1[i] = 0;
    ap1_out_delayline1[i] = 0;
    ap2_in_delayline1[i] = 0;
    ap2_out_delayline1[i] = 0;
  }
  for(i = 0; i < AP_DELAY_LENGTH2/2; i++)
  {
    ap1_in_delayline2[i] = 0;
    ap1_out_delayline2[i] = 0;
    ap2_in_delayline2[i] = 0;
    ap2_out_delayline2[i] = 0;
  }
  for(i = 0; i < AP_DELAY_LENGTH3/2; i++)
  {
    ap1_in_delayline3[i] = 0;
    ap1_out_delayline3[i] = 0;
    ap2_in_delayline3[i] = 0;
    ap2_out_delayline3[i] = 0;
  }
  for(i = 0; i < AP_DELAY_LENGTH4/2; i++)
  {
    ap1_in_delayline4[i] = 0;
    ap1_out_delayline4[i] = 0;
    ap2_in_delayline4[i] = 0;
    ap2_out_delayline4[i] = 0;
  }

}
#endif

void setup() {

  Serial.begin(115200);

  delay(1000);

  Serial.println("...starting init...");
  
  AudioMemory(30);

  AudioNoInterrupts();

  //cleanBuffers();

  Serial.println("...bufffers cleaned...");
  
  sgtl5000_1.enable();
  
  sgtl5000_1.volume(0.75);

  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
  sgtl5000_1.lineInLevel(1);


#if 0
  SPI.setMOSI(7);
  SPI.setSCK(14);
  if (SD.begin(10)) {
    playWav1.play("SDTEST1.WAV");
  }
#endif
  
  lbcf1.setCoefficients(0, coeffs, delayline1, LBCF_DELAY_LENGTH1/2);
  lbcf2.setCoefficients(0, coeffs, delayline2, LBCF_DELAY_LENGTH2/2);
  lbcf3.setCoefficients(0, coeffs, delayline3, LBCF_DELAY_LENGTH3/2);
  lbcf4.setCoefficients(0, coeffs, delayline4, LBCF_DELAY_LENGTH4/2);
  lbcf5.setCoefficients(0, coeffs, delayline5, LBCF_DELAY_LENGTH5/2);
  lbcf6.setCoefficients(0, coeffs, delayline6, LBCF_DELAY_LENGTH6/2);
  lbcf7.setCoefficients(0, coeffs, delayline7, LBCF_DELAY_LENGTH7/2);
  lbcf8.setCoefficients(0, coeffs, delayline8, LBCF_DELAY_LENGTH8/2);
  lbcf9.setCoefficients(0, coeffs, delayline1, LBCF_DELAY_LENGTH1/2);
  lbcf10.setCoefficients(0, coeffs, delayline2, LBCF_DELAY_LENGTH2/2);
  lbcf11.setCoefficients(0, coeffs, delayline3, LBCF_DELAY_LENGTH3/2);
  lbcf12.setCoefficients(0, coeffs, delayline4, LBCF_DELAY_LENGTH4/2);
  lbcf13.setCoefficients(0, coeffs, delayline5, LBCF_DELAY_LENGTH5/2);
  lbcf14.setCoefficients(0, coeffs, delayline6, LBCF_DELAY_LENGTH6/2);
  lbcf15.setCoefficients(0, coeffs, delayline7, LBCF_DELAY_LENGTH7/2);
  lbcf16.setCoefficients(0, coeffs, delayline8, LBCF_DELAY_LENGTH8/2);

#if 1
  ap1.setCoefficients(0, g, ap1_in_delayline1, ap1_out_delayline1, AP_DELAY_LENGTH1/2);
  ap1.setCoefficients(1, g, ap1_in_delayline2, ap1_out_delayline2, AP_DELAY_LENGTH2/2);
  ap1.setCoefficients(2, g, ap1_in_delayline3, ap1_out_delayline3, AP_DELAY_LENGTH3/2);
  ap1.setCoefficients(3, g, ap1_in_delayline4, ap1_out_delayline4, AP_DELAY_LENGTH4/2);
  ap2.setCoefficients(0, g, ap2_in_delayline1, ap2_out_delayline1, AP_DELAY_LENGTH1/2);
  ap2.setCoefficients(1, g, ap2_in_delayline2, ap2_out_delayline2, AP_DELAY_LENGTH2/2);
  ap2.setCoefficients(2, g, ap2_in_delayline3, ap2_out_delayline3, AP_DELAY_LENGTH3/2);
  ap2.setCoefficients(3, g, ap2_in_delayline4, ap2_out_delayline4, AP_DELAY_LENGTH4/2);
#endif
  
  mixer1.gain(0, 0.25);
  mixer1.gain(1, 0.25);
  mixer1.gain(2, 0.25);
  mixer1.gain(3, 0.25);
  
  mixer2.gain(0, 0.25);
  mixer2.gain(1, 0.25);
  mixer2.gain(2, 0.25);
  mixer2.gain(3, 0.25);
  
  mixer4.gain(0, 0.25);
  mixer4.gain(1, 0.25);
  mixer4.gain(2, 0.25);
  mixer4.gain(3, 0.25);
  
  mixer5.gain(0, 0.25);
  mixer5.gain(1, 0.25);
  mixer5.gain(2, 0.25);
  mixer5.gain(3, 0.25);
  
  mixer3.gain(0,0.5);
  mixer3.gain(1,0.5);
  
  mixer8.gain(0,0.5);
  mixer8.gain(1,0.5);
  
  mixer4.gain(0, 0.30);
  mixer4.gain(1, 0.20);
  mixer4.gain(2, 0.30);
  mixer4.gain(3, 0.20);

  next = millis() + 1000;

  AudioInterrupts();

  Serial.println("Init complete.");
}
  
void loop() {

  //Serial.print(next);
  //Serial.print(" ");
  //Serial.println(millis());
  //delay(100);
  
  if(millis() >= next)
  {
    next = millis() + 1000;

    Serial.print("Diagnostics ");
    Serial.print(AudioProcessorUsageMax());
    Serial.print(" ");
    Serial.println(AudioMemoryUsageMax());
    AudioProcessorUsageMaxReset();
  }


}
  
