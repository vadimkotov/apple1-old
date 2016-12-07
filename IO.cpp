// #include <stdio.h>
#include <stdint.h>
#include <ncurses.h>

#include "IO.h"
#include "RAM.h"



IO::IO(RAM* ram) {

  this->ram = ram;
  

  /*
  this->piaKbd = 0x80;
  this->piaKbdCr = 0;
  this->piaDsp = 0;
  this->piaDspCr = 0;
  */

  if ((this->window = initscr()) == NULL) {
    // TODO: throw exception
  }

  if (wresize(this->window, TEXT_HEIGHT, TEXT_WIDTH) == ERR) {
    // TODO: throw exception
  }
  
  nonl();
  cbreak();
  noecho();
  nodelay(this->window, true);

}

IO::~IO() {
  endwin();
}


void IO::reset() {
  // PA7 (i.e. bit 7 of KBD) is hardwired to +5V
  // this->ram->write(PIA_KBD, 0x80);
  this->ram->write(PIA_KBD, 0);
  this->ram->write(PIA_KBD_CR, 0);
  this->ram->write(PIA_DSP, 0);
  this->ram->write(PIA_DSP_CR, 0);
}

void IO::update() {

  uint8_t ch;
  uint8_t dsp = this->ram->read(PIA_DSP);

  
  // If there's data to be displayed (b7)
  if (dsp & 0x80) {
    // Ignore b7 as it's a status bit
    dsp &= 0x7F;
    
    ch = dsp;
    
    // Uppercase
    if (dsp >= 0x60 && dsp <= 0x7F)
      ch &= 0x5F;

    // Get the cursor position
    int x, y;
    getyx(this->window, y, x);
    
    if (ch == '\r') {
      move(++y, 0);
    } else if (ch >= ' ' && ch <= '_') {
      addch(ch);
      ++x;
    } else if (!ch) {
      addch('@');
      move(y, x);
    } 
    
    refresh();
    
    this->ram->write(PIA_DSP, dsp);
  }

  /*
  ch = getch();  
  if (ch != ERR && this->ram->read(PIA_KBD_CR) == 0x27) {

    // Uppercase
    if (ch >= 0x60 && ch <= 0x7a)
      ch &= 0x5F;

    if (ch == '\n')
      ch = 0x0D;

    if (ch < 0x60) {
      this->ram->write(PIA_KBD, 0x80 | ch);
      this->ram->write(PIA_KBD_CR, 0xA7);
    }
  }
  */
}
