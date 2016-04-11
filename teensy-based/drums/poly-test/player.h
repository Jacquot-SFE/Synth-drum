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

  void tick();

private:

  bool     playing;
  uint32_t current_step;
  uint32_t prev_step;
  uint32_t pause_len;
  uint32_t next_time;
  

};



#endif // keepout
