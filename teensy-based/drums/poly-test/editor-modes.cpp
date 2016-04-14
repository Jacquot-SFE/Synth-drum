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

// Local definitions

static const int32_t PATTERN_SEL_INDICATOR = 0x10;
static const int32_t STEP_EDIT_INDICATOR = 0x11;
static const int32_t VOICE_SEL_INDICATOR = 0x12;
static const int32_t MUTE_SEL_INDICATOR = 0x13;
static const int32_t PLAY_INDICATOR = 0x17;

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

  if (keynum == PLAY_INDICATOR) //start/stop key
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
  else if(keynum == VOICE_SEL_INDICATOR) // voice select mode
  {
    if(pressed)
    {
      theEditor.setMode(Editor::eMODE_VOICE_SEL);
    }
  }
  else if(keynum == MUTE_SEL_INDICATOR)
  {
    if(pressed)
    {
      theEditor.setMode(Editor::eMODE_MUTE_SEL);
    }
  }
  else if(keynum == PATTERN_SEL_INDICATOR) // pattern select mode
  {
    if(pressed)
    {
      theEditor.setMode(Editor::eMODE_PATT_SEL);
    }
  }
#if 0
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
    theScanner.setBackgroundLED(STEP_EDIT_INDICATOR);
    
    for(uint32_t i = 0; i < Pattern::PATTERN_LEN; i++)
    {
      if(thePattern.getVoiceBit(i))
      {
        theScanner.setBackgroundLED(i);
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
  if(keynum == VOICE_SEL_INDICATOR) // voice select mode
  {
    if(!pressed)
    {
      theEditor.setMode(Editor::eMODE_STEP_EDIT);
    }
  }
  else if (keynum == PLAY_INDICATOR) //start/stop key
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
    theScanner.setBackgroundLED(VOICE_SEL_INDICATOR);

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
  
  if(keynum == STEP_EDIT_INDICATOR) // voice select mode
  {
    if(pressed)
    {
      theEditor.setMode(Editor::eMODE_STEP_EDIT);
    }
  }
  else if(keynum == PATTERN_SEL_INDICATOR) // pattern select mode
  {
    if(pressed)
    {
      theEditor.setMode(Editor::eMODE_PATT_SEL);
    }
  }
  else if (keynum == PLAY_INDICATOR) //start/stop key
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
    theScanner.setBackgroundLED(MUTE_SEL_INDICATOR);

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
  if(keynum == STEP_EDIT_INDICATOR)
  {
    if(pressed)
    {
      theEditor.setMode(Editor::eMODE_STEP_EDIT);
    }
  }
  else if(keynum == MUTE_SEL_INDICATOR)
  {
    if(pressed)
    {
      theEditor.setMode(Editor::eMODE_MUTE_SEL);
    }
  }
  else if (keynum == PLAY_INDICATOR) //start/stop key
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
    theScanner.setBackgroundLED(PATTERN_SEL_INDICATOR);
    theScanner.setBackgroundLED(thePattern.getCurrentPattern());
  }
  else
  {
    theScanner.clearAllBackgroundLEDs();
    theScanner.clearAllBlinkingLEDs();
  }

}


