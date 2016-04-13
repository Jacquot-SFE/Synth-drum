#include <arduino.h>

#include "editor.h"
#include "player.h"

extern PanelScanner theScanner;
extern Player       thePlayer;


// decls for the mode objects
static StepEdit stepEditor;
static VoiceSelect voiceSelector;
static MuteSelect muteSelector;

// constructor...
Editor::Editor()
{
  current_mode_p = &stepEditor;
  current_mode_p->setLEDs(true);
}

void Editor::setMode(EditorMode newmode)
{
  switch(newmode)
  {
  case eMODE_VOICE_SEL:
    {
      current_mode_p->setLEDs(false);
      current_mode_p = &voiceSelector;
      current_mode_p->setLEDs(true);
      break;
    }
  case eMODE_MUTE_SEL:
    {
      current_mode_p->setLEDs(false);
      current_mode_p = &muteSelector;
      current_mode_p->setLEDs(true);
      break;
    }
  default:
    {
      current_mode_p->setLEDs(false);
      current_mode_p = &stepEditor;
      current_mode_p->setLEDs(true);
      break;
    }
  }
  
}

void Editor::receiveKey(uint32_t keynum, bool pressed)
{
#if 1
  Serial.print("Ed Key: ");
  Serial.print(keynum, HEX);
  Serial.print(" ");
  Serial.println(pressed);
#endif

  current_mode_p->HandleKey(keynum, pressed);
}

void Editor::forceLEDs()
{
  current_mode_p->setLEDs(true);  
}

