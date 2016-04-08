#include <SPI.h>
#include <arduino.h>

#include "panel-scanner.h"

static const int32_t CHIPSEL_BTNS = 8;
static const int32_t CHIPSEL_LEDS = 5;

static const SPISettings registersettings(500000, MSBFIRST, SPI_MODE0 );


// constructor...
PanelScanner::PanelScanner()
{
  for(uint32_t i = 0; i < NUM_PANELS; i++)
  {
    output_buffer[i] = 0;
    input_buffer[i] = 0;
  }
}

void PanelScanner::initScanning()
{
  SPI.setSCK(14);
  SPI.setMOSI(7);
  
  pinMode(CHIPSEL_BTNS, OUTPUT);
  digitalWrite(CHIPSEL_BTNS, LOW);
  pinMode(CHIPSEL_LEDS, OUTPUT);
  digitalWrite(CHIPSEL_LEDS, LOW);

  SPI.begin();

}

void PanelScanner::setLED(uint32_t num)
{
  uint32_t byte_idx, bit_num;
  
  if(num < (NUM_PANELS*8))
  {
    byte_idx = num / 8;
    bit_num = num % 8;

    output_buffer[byte_idx] |= 1 << bit_num;
  }
}

void PanelScanner::clearLED(uint32_t num)
{
  uint32_t byte_idx, bit_num;
  
  if(num < (NUM_PANELS*8))
  {
    byte_idx = num / 8;
    bit_num = num % 8;

    output_buffer[byte_idx] &= ~(1 << bit_num);
  }
}


void PanelScanner::doTransaction()
{
  uint32_t i;
  
  for(i = 0; i < NUM_PANELS; i++)
  {
    trans_buffer[i] = output_buffer[i];
    Serial.println(trans_buffer[i], HEX);
  }

  SPI.beginTransaction(registersettings);

  SPI.transfer(trans_buffer, NUM_PANELS);

  digitalWrite(CHIPSEL_LEDS, HIGH);
  digitalWrite(CHIPSEL_BTNS, LOW);
  digitalWrite(CHIPSEL_LEDS, LOW);

  SPI.endTransaction();

  digitalWrite(CHIPSEL_BTNS, HIGH);

  for(i = 0; i < NUM_PANELS; i++)
  {
    input_buffer[i] = trans_buffer[i];
  }
  

}

