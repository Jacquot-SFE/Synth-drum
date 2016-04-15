#include <Arduino.h>
//#include <>

#include "player.h"
#include "pattern.h"
#include "panel-scanner.h"
#include "editor.h"

// TBD: clean this mess up
// ugly way to get to the voices...
#include <Audio.h>
#include "Synth-Clatter.h"
#include "Synth-Decay.h"
#include "Synth-DrumHeart.h"

extern AudioSynthSimpleDrum kick;
extern AudioSynthDrumHeart  snare;
extern AudioSynthSimpleDrum tom;
extern AudioSynthDecay      shakedecay;
extern AudioSynthDecay      hatdecay;
extern AudioSynthDecay      belldecay;

extern uint16_t openlen, closedlen;
extern uint16_t t1, t2, t3;


#define HAT
#define KICK
#define SNARE
#define TOM
#define SHAKER
#define BELL


// Similar, for the pattern...
extern Pattern      thePattern;
extern PanelScanner theScanner;
extern Editor       theEditor;

// constructor...
Player::Player()
{
  playing = false;
  swing   = false;
  current_step = 0;

  active_mutes = 0;
  pending_mutes = 0;

  active_pattern = 0;
  pending_pattern = -1;

  pause_len = 125;// milliseconds, 125 mS = 120 bpm
}

void Player::start()
{
  playing = true;
  current_step = 0;
  theScanner.setOverlayLED(0x17);
}

void Player::stop()
{
  playing = false;
  theScanner.clearOverlayLED(0x17);
  theScanner.clearOverlayLED(prev_step);
}

bool Player::isPlaying()
{
  return playing;
}

void Player::setPause(uint32_t millisec)
{
  pause_len = millisec;
}

bool Player::toggleSwing()
{
  swing = !swing;

  Serial.print("Swing is now: ");
  Serial.println(swing);

  return swing;
}

bool Player::getSwing()
{
  return swing;
}

bool Player::toggleMuteBit(uint32_t bit)
{
  if(bit > 8)
    return false;

  if(playing)
  {
    pending_mutes ^= (1 << bit);

    return (pending_mutes & (1 << bit));
  }
  else
  {
    active_mutes ^= (1 << bit);

    return (active_mutes & (1 << bit));
  }
}
 
bool Player::getMuteBit(uint32_t bit)
{
  if(bit > 8)
    return false;

  return (active_mutes & (1 << bit));
}

bool Player::setNextPattern(uint32_t next)
{
  if(next > Pattern::NUM_PATTERNS)
    return false;

  if(next == active_pattern)
  {
    return false;
  }

  if(playing)
  {
    pending_pattern = next;

    return true;// (pending_pattern);
  }
  else
  {
    active_pattern = next;
    thePattern.setCurrentPattern(active_pattern);

    return true;  //(active_mutes & (1 << bit));
  }
}

int32_t Player::getActivePattern()
{
  return active_pattern;
}


void Player::tick()
{
  uint32_t now = millis();
  
  if(!playing)
  {
    return;  
  }

  if(now < next_time)
  {
    return;
  }

  if(!swing)
  {
    next_time = now + pause_len;
  }
  else
  {
    if(!(current_step & 0x01))
    {
      next_time = now + pause_len + (pause_len/3);
    }
    else
    {
      next_time = now + ((2*pause_len)/3);
    }
  }
   
  //
  uint8_t trigdata = thePattern.getStepData(current_step);

  // Apply mutes
  trigdata &= (~active_mutes);

  theScanner.clearOverlayLED(prev_step);
  theScanner.setOverlayLED(current_step);
  //theScanner.doTransaction();

#if 0
  Serial.print("Trigger: step#");
  Serial.print(current_step);
  Serial.print(" bitmap:");
  Serial.println(trigdata, HEX);
#endif

  AudioNoInterrupts();

#ifdef KICK
  if (trigdata & 0x01)
    kick.noteOn();
#endif    
#ifdef SNARE
  if (trigdata & 0x02)
    snare.noteOn();
#endif
#ifdef HAT
  if (trigdata & 0x04)
  {
    hatdecay.length(closedlen);
    hatdecay.noteOn();
  }
  else if (trigdata & 0x08)
  {
    hatdecay.length(openlen);
    hatdecay.noteOn();
  }
#endif
#ifdef TOM  
  if (trigdata & 0x10)
  {
    tom.frequency(t1);
    tom.noteOn();
  }
  else if (trigdata & 0x20)
  {
    tom.frequency(t2);
    tom.noteOn();
  }
  else if (trigdata & 0x40)
  {
    tom.frequency(t3);
    tom.noteOn();
  }
#endif
#ifdef SHAKER  
  if (trigdata & 0x80)
  {
    //shakedecay.noteOn();
    belldecay.noteOn();
  }
#endif
  AudioInterrupts();

  // Keep track of previous step so we can turn
  // it's LED off
  prev_step = current_step; 
  current_step++;
  if(current_step > 0x0f)
  {
    current_step = 0;
    if(pending_mutes)
    {
      active_mutes ^= pending_mutes;
      pending_mutes = 0;

      theScanner.clearAllBlinkingLEDs();
      theEditor.forceLEDs();
    }

    if(pending_pattern != -1)
    {
      active_pattern = pending_pattern;
      thePattern.setCurrentPattern(pending_pattern);
      pending_pattern = -1;

      theScanner.clearAllBlinkingLEDs();
      theScanner.clearAllBackgroundLEDs();
      theEditor.forceLEDs();
    }
  }
}

