#ifndef _PANEL_SCAN_H_
#define _PANEL_SCAN_H_

#pragma once

static const uint32_t NUM_PANELS = 3;

class PanelScanner
{
public: 
  PanelScanner();

  void initScanning();

  void setLED(uint32_t num);
  void clearLED(uint32_t num);

  void doTransaction();


private:


  uint8_t trans_buffer[NUM_PANELS];
  uint8_t output_buffer[NUM_PANELS];
  uint8_t input_buffer[NUM_PANELS];
  
};



#endif // keepout
