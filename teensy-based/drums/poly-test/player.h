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

  bool toggleMuteBit(uint32_t bit);
  bool getMuteBit(uint32_t bit);

  void tick();

private:

  bool     playing;
  uint32_t current_step;
  uint32_t prev_step;
  uint32_t pause_len;
  uint32_t next_time;

  uint32_t active_mutes;
  uint32_t pending_mutes;

};



#endif // keepout
