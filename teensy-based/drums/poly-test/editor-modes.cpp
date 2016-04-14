#include <Arduino.h>

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
  bool setting;

  if (keynum == 0x17) //start/stop key
  {
    HandlePlayButton(pressed);
  }
  else if((keynum >= 0) && (keynum <= 15))
  {
    if(pressed)
    {
      setting = thePattern.toggleBit(keynum);
      theScanner.setBackgroundLED(keynum, setting);
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
  else if(keynum == 0x11) // voice mute select 
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
  else if(keynum == 0x12) // pattern select mode
  {
    if(pressed)
    {
      theEditor.setMode(Editor::eMODE_PATT_SEL);
    }
    else
    {
      theEditor.setMode(Editor::eMODE_STEP_EDIT);
    }
  }
#if 1
  else// other, unmapped keys, just show they work.
  {
    theScanner.setBackgroundLED(keynum, pressed);
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
      theScanner.setBackgroundLED(i, thePattern.getVoiceBit(i));
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
    // Mode indication
    theScanner.setBackgroundLED(0x10);

    // Present voice indication
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
  bool setting;
  
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
      setting = thePlayer.toggleMuteBit(keynum);
      
      if(thePlayer.isPlaying())
      {
        theScanner.setBlinkingLED(keynum, setting);
      }
      else
      {
        theScanner.setBackgroundLED(keynum, setting);
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
      theScanner.setBackgroundLED(i, thePlayer.getMuteBit(i));
    }
  }
  else
  {
    theScanner.clearAllBackgroundLEDs();
    theScanner.clearAllBlinkingLEDs();
  }
}

///////////////////////////////////////////////////////////////////////////////////////
//////// Pattern Selector
///////////////////////////////////////////////////////////////////////////////////////


PatternSelect::PatternSelect()
{
  
}
  
void PatternSelect::HandleKey(uint32_t keynum, bool pressed)
{
  if(keynum == 0x12) //exit pattern select - to step edit
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
  else if((keynum >= 0) && (keynum <= 15))
  {
    if(pressed)
    {
      if(thePlayer.isPlaying())
      {
          // returns true if next pattern is different
          if(thePlayer.setNextPattern(keynum))
          {
            theScanner.setBlinkingLED(keynum);
          }
      }
      else
      {
        theScanner.clearBackgroundLED(thePlayer.getActivePattern());
        thePlayer.setNextPattern(keynum);
        theScanner.setBackgroundLED(keynum);
      }
    }
  }
}

void PatternSelect::setLEDs(bool entry)
{
  if(entry)
  {
    // set mode indicator
    theScanner.setBackgroundLED(0x12);
    theScanner.setBackgroundLED(thePattern.getCurrentPattern());
  }
  else
  {
    theScanner.clearAllBackgroundLEDs();
    theScanner.clearAllBlinkingLEDs();
  }

}


