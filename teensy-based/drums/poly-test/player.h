#ifndef _PLAYER_H_
#define _PLAYER_H_

#pragma once

class Player
{
public: 
  Player();

  void start();
  void stop();

  bool isPlaying();

  void setPause(uint32_t millisec);

  bool toggleSwing();
  bool getSwing();

  bool toggleMuteBit(uint32_t bit);
  bool getMuteBit(uint32_t bit);

  bool setNextPattern(uint32_t next);
  int32_t getActivePattern();

  void tick();

private:

  bool    playing;
  bool    swing;
  int32_t current_step;
  int32_t prev_step;
  int32_t pause_len;
  int32_t next_time;

  int32_t active_mutes;
  int32_t pending_mutes;

  int32_t active_pattern;
  int32_t pending_pattern;

};



#endif // keepout
