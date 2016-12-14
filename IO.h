#pragma once

#include <stdint.h>
#include <ncurses.h>
#include "RAM.h"

/*
  This class combines emulated PIA,
  handles keyboard input and and
  writes to stdout
*/


/*
[ PIA VARIABLES ]

KBD     $D010 - holds the ASCII character received
                from the keyboard
KBD CR  $D011 - keyboard control register
DSP     $D012 - ASCII character to be sent to the 
                display
DSP CR  $D013 - display  control register
*/

#define PIA_KBD    0xD010
#define PIA_KBD_CR 0xD011
#define PIA_DSP    0xD012
#define PIA_DSP_CR 0xD013

/*
  Bits of a control register (PIA has two)
*/
#define PIA_CONTROL_1  0x03 // 2 bits
#define PIA_DDR_ACCESS 0x04 // 1 bit
#define PIA_CONTROL_2  0x38 // 3 bits
#define PIA_IRQ_1      0x40 // 1 bit
#define PIA_IRQ_2      0x80 // bit



#define TEXT_WIDTH  40
#define TEXT_HEIGHT 24

class IO {
 public:
  IO(RAM* ram);
  ~IO();

  void reset();
  void update();

 private:
  RAM* ram;
  WINDOW* window;
  
  void updateDisplay(uint8_t dsp);
  void updateKeyboard();

};
