#include "editor-modes.h"
#include "editor.h"
#include "panel-scanner.h"
#include "player.h"
#include "pattern.h"

extern PanelScanner theScanner;
extern Player       thePlayer;
extern Pattern      thePattern;
extern Editor       theEditor;

///////////////////////////////////////////////////////////////////////////////////////
//////// Base Class
///////////////////////////////////////////////////////////////////////////////////////

// constructor...
pvEditorMode::pvEditorMode()
{
}

// Common handling for the start/stop key
void pvEditorMode::HandlePlayButton(bool pressed)
{
  if(pressed )
  {
    if(thePlayer.isPlaying())
    {
      thePlayer.stop();
    }
    else
    {
      thePlayer.start();
      //theScanner.setOverlayLED(0x17);
    }
  }
}


///////////////////////////////////////////////////////////////////////////////////////
//////// Step Editor
///////////////////////////////////////////////////////////////////////////////////////

StepEdit::StepEdit(): pvEditorMode()
{
  //current_voice = 0;
  
}

void StepEdit::HandleKey(uint32_t keynum, bool pressed)
{

  if (keynum == 0x17) //start/stop key
  {
    HandlePlayButton(pressed);
  }
  else if((keynum >= 0) && (keynum <= 15))
  {
    if(pressed)
    {
      if(thePattern.toggleBit(keynum))
      {
        theScanner.setBackgroundLED(keynum);
      }
      else
      {
        theScanner.clearBackgroundLED(keynum);
      }
    }
  }
  else if(keynum == 0x10) // voice select mode
  {
    if(pressed)
    {
      theEditor.setMode(Editor::eMODE_VOICE_SEL);
    }
    else
    {
      theEditor.setMode(Editor::eMODE_STEP_EDIT);
    }
  }
  else if(keynum == 0x11) // voice select mode
  {
    if(pressed)
    {
      theEditor.setMode(Editor::eMODE_MUTE_SEL);
    }
    else
    {
      theEditor.setMode(Editor::eMODE_STEP_EDIT);
    }
  }
#if 1
  else// other, unmapped keys, just show they work.
  {
    if(pressed)
    {
      theScanner.setBackgroundLED(keynum);
    }
    else
    {
      theScanner.clearBackgroundLED(keynum);
    }
  }
#endif    
  
}

void StepEdit::setLEDs(bool entry)
{
  // Initialize LEDs 
  if(entry)
  {
    for(uint32_t i = 0; i < Pattern::PATTERN_LEN; i++)
    {
      if(thePattern.getVoiceBit(i))
      {
        theScanner.setBackgroundLED(i);
      }
      else
      {
        theScanner.clearBackgroundLED(i);
      }
    }
  }
  else // on exit
  {
    theScanner.clearAllBackgroundLEDs();
    theScanner.clearAllBlinkingLEDs();
  }
  
}


///////////////////////////////////////////////////////////////////////////////////////
//////// Voice Selector
///////////////////////////////////////////////////////////////////////////////////////

VoiceSelect::VoiceSelect(): pvEditorMode()
{
  
}

void VoiceSelect::HandleKey(uint32_t keynum, bool pressed)
{
  if(keynum == 0x10) // voice select mode
  {
    if(!pressed)
    {
      theEditor.setMode(Editor::eMODE_STEP_EDIT);
    }
  }
  else if (keynum == 0x17) //start/stop key
  {
    HandlePlayButton(pressed);
  }
  else if((keynum >= 0) && (keynum <= 7))
  {
    // TBD - more voices means accept more input
    if(pressed)
    {
      theScanner.clearBackgroundLED(thePattern.getCurrentVoice());
      thePattern.setCurrentVoice(keynum);
      theScanner.setBackgroundLED(keynum);
    }
  }
}

void VoiceSelect::setLEDs(bool entry)
{
  uint32_t bitnum;
  
  if(entry)
  {
    theScanner.setBackgroundLED(0x10);
    
    bitnum = thePattern.getCurrentVoice();
    theScanner.setBackgroundLED(bitnum);
  }
  else
  {
    theScanner.clearAllBackgroundLEDs();
  }
}


///////////////////////////////////////////////////////////////////////////////////////
//////// Mute Selector
///////////////////////////////////////////////////////////////////////////////////////

MuteSelect::MuteSelect(): pvEditorMode()
{
  
}

void MuteSelect::HandleKey(uint32_t keynum, bool pressed)
{
  if(keynum == 0x11) // voice select mode
  {
    if(pressed)
    {
      theEditor.setMode(Editor::eMODE_STEP_EDIT);
    }
  }
  else if (keynum == 0x17) //start/stop key
  {
    HandlePlayButton(pressed);
  }
  else if((keynum >= 0) && (keynum <= 7))
  {
    if(pressed)
    {
      if(thePlayer.isPlaying())
      {
        if(thePlayer.toggleMuteBit(keynum))
        {
          theScanner.setBlinkingLED(keynum);
        }
        else
        {
          theScanner.clearBlinkingLED(keynum);
        }
      }
      else
      {
        if(thePlayer.toggleMuteBit(keynum))
        {
          theScanner.setBackgroundLED(keynum);
        }
        else
        {
          theScanner.clearBackgroundLED(keynum);
        }
        
      }
    }
  }
}

void MuteSelect::setLEDs(bool entry)
{
  if(entry)
  {
    // set mode indicator
    theScanner.setBackgroundLED(0x11);

    // and siaplay data on editor buttons
    for(uint32_t i = 0; i < 8; i++)
    {
      if(thePlayer.getMuteBit(i))
      {
        theScanner.setBackgroundLED(i);
      }
      else
      {
        theScanner.clearBackgroundLED(i);
      }
    }
  }
  else
  {
    theScanner.clearAllBackgroundLEDs();
    theScanner.clearAllBlinkingLEDs();
  }
}

