#include <SPI.h>
#include <arduino.h>

#include "panel-scanner.h"

static const int32_t CHIPSEL_BTNS = 8;
static const int32_t CHIPSEL_LEDS = 5;

extern Editor       theEditor;

static const SPISettings registersettings(500000, MSBFIRST, SPI_MODE0 );


// constructor...
PanelScanner::PanelScanner()
{
  for(uint32_t i = 0; i < NUM_PANELS; i++)
  {
    led_background_buffer[i] = 0;
    led_overlay_buffer[i] = 0;
    old_buttons[i] = 0;
    new_buttons[i] = 0;
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

  // Apparently the first scan returns junk?
  doTransaction();
}

void PanelScanner::tick()
{
  doTransaction();
  parseButtons();
}

void PanelScanner::clearAllLED()
{
  for(uint32_t i = 0; i < NUM_PANELS; i++)
  {
    led_background_buffer[i] = 0;
    led_overlay_buffer[i] = 0;
  }
}


void PanelScanner::setBackgroundLED(uint32_t num)
{
  uint32_t byte_idx, bit_num;

  // Funny math at play here.
  // LEDs are out of order WRT the buttons...the first bit shifted in is the 
  // last bit shifted out on the SPI ring.
  // So we'll flop that around here by doing the shifting and indexing
  // math from the top down, rather than bottom up.
  
  if(num < (NUM_PANELS*8))
  {
    byte_idx = NUM_PANELS - 1 -(num / 8);
    bit_num = num % 8;

    led_background_buffer[byte_idx] |= 0x80 >> bit_num;
  }
}

void PanelScanner::clearBackgroundLED(uint32_t num)
{
  uint32_t byte_idx, bit_num;

  // see note above.
  
  if(num < (NUM_PANELS*8))
  {
    byte_idx = NUM_PANELS - 1 -(num / 8);
    bit_num = num % 8;

    led_background_buffer[byte_idx] &= ~(0x80 >> bit_num);
  }
}

void PanelScanner::clearAllBackgroundLEDs()
{
    for(uint32_t i = 0; i < NUM_PANELS; i++)
  {
    led_background_buffer[i] = 0;
  }
}

void PanelScanner::setOverlayLED(uint32_t num)
{
  uint32_t byte_idx, bit_num;

  // Funny math at play here.
  // LEDs are out of order WRT the buttons...the first bit shifted in is the 
  // last bit shifted out on the SPI ring.
  // So we'll flop that around here by doing the shifting and indexing
  // math from the top down, rather than bottom up.
  
  if(num < (NUM_PANELS*8))
  {
    byte_idx = NUM_PANELS - 1 -(num / 8);
    bit_num = num % 8;

    led_overlay_buffer[byte_idx] |= 0x80 >> bit_num;
  }
}

void PanelScanner::clearOverlayLED(uint32_t num)
{
  uint32_t byte_idx, bit_num;

  // see note above.
  
  if(num < (NUM_PANELS*8))
  {
    byte_idx = NUM_PANELS - 1 -(num / 8);
    bit_num = num % 8;

    led_overlay_buffer[byte_idx] &= ~(0x80 >> bit_num);
  }
}

/*
void PanelScanner::readButtons()
{
  uint32_t i;
  uint8_t trans_buffer[NUM_PANELS] = {0,0,0};

  SPI.beginTransaction(registersettings);

  SPI.transfer(trans_buffer, NUM_PANELS);

  digitalWrite(CHIPSEL_BTNS, LOW);

  SPI.endTransaction();

  digitalWrite(CHIPSEL_BTNS, HIGH);

  for(i = 0; i < NUM_PANELS; i++)
  {
    // Button lines are pulled high by resistors, shorted 
    // to ground when button is pressed.
    new_buttons[i] = ~trans_buffer[i];
  }
}
*/
void PanelScanner::parseButtons()
{
  uint8_t diff;
  
  for(uint32_t i = 0; i < NUM_PANELS; i++)
  {
    // in each byte, see if anything changed from last invocation
    diff = new_buttons[i] ^ old_buttons[i];
    
    for(uint32_t j = 0, mask = 0x01; j < 8; j++, mask<<=1)
    {
      if(diff & mask)
      {
          //Serial.println(new_buttons[i]);
          theEditor.receiveKey((i*8)+j, (new_buttons[i] & mask));
      }
    }

    // then store the new state for comparison next time
    old_buttons[i] = new_buttons[i];
  }
}

/*
void PanelScanner::writeLEDs()
{
  uint32_t i;
  uint8_t trans_buffer[NUM_PANELS];
  
  for(i = 0; i < NUM_PANELS; i++)
  {
    trans_buffer[i] = led_buffer[i];
    //Serial.println(trans_buffer[i], HEX);
  }

  SPI.beginTransaction(registersettings);

  SPI.transfer(trans_buffer, NUM_PANELS);

  digitalWrite(CHIPSEL_LEDS, HIGH);

  SPI.endTransaction();

  digitalWrite(CHIPSEL_LEDS, LOW);

}
*/

void PanelScanner::doTransaction()
{
  uint32_t i;
  uint8_t trans_buffer[NUM_PANELS];
  
  for(i = 0; i < NUM_PANELS; i++)
  {
    trans_buffer[i] = led_background_buffer[i] ^ led_overlay_buffer[i];
//    Serial.println(trans_buffer[i], HEX);
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
    // Button lines are pulled high by resistors, shorted 
    // to ground when button is pressed.
    new_buttons[i] = ~trans_buffer[i];
  }
}

