#ifndef _EDITOR_MODES_H_
#define _EDITOR_MODES_H_

#pragma once

#include <stdint.h>

// fwd decl for other headers
class pvEditorMode
{
  public:

    pvEditorMode();

    virtual void HandleKey(uint32_t keynum, bool pressed) = 0;
    virtual void setLEDs(bool entry) = 0;

  private:
};


class StepEdit: public pvEditorMode
{
  public:

    StepEdit();
    virtual void HandleKey(uint32_t keynum, bool pressed);
    virtual void setLEDs(bool entry);
  private:

};

class VoiceSelect: public pvEditorMode
{
  public:

    VoiceSelect();
    virtual void HandleKey(uint32_t keynum, bool pressed);
    virtual void setLEDs(bool entry);

  private:
};

class MuteSelect: public pvEditorMode
{
  public:

    MuteSelect();
    virtual void HandleKey(uint32_t keynum, bool pressed);
    virtual void setLEDs(bool entry);

  private:
};


#endif // keepout
