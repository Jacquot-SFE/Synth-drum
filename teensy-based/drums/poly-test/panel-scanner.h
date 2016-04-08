#ifndef _PANEL_SCANNER_H_
#define _PANEL_SCANNER_H_

#pragma once

#include "editor.h"

static const uint32_t NUM_PANELS = 3;

class PanelScanner
{
public: 
  PanelScanner();

  void initScanning();

  void tick();

  void setLED(uint32_t num);
  void clearLED(uint32_t num);

  void doTransaction();


private:

  void readButtons();
  void parseButtons();

  void writeLEDs();

  uint8_t led_buffer[NUM_PANELS];
  uint8_t old_buttons[NUM_PANELS];
  uint8_t new_buttons[NUM_PANELS];

};



#endif // keepout
