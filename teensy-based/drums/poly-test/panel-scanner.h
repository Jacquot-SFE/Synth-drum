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

  void clearAllLED();

  void setBackgroundLED(uint32_t num);
  void clearBackgroundLED(uint32_t num);
  void clearAllBackgroundLEDs();
  void setOverlayLED(uint32_t num);
  void clearOverlayLED(uint32_t num);

  void doTransaction();


private:

  void readButtons();
  void parseButtons();

  void writeLEDs();

  uint8_t led_background_buffer[NUM_PANELS];
  uint8_t led_overlay_buffer[NUM_PANELS];
  uint8_t old_buttons[NUM_PANELS];
  uint8_t new_buttons[NUM_PANELS];

};



#endif // keepout
