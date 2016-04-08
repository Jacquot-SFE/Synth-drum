#ifndef _EDITOR_H_
#define _EDITOR_H_

#pragma once

// fwd decl for other headers
class Editor;

#include "panel-scanner.h"

class Editor
{
public: 
  Editor();

  void receiveKey(uint32_t keynum, bool pressed);
  
private:

};



#endif // keepout
