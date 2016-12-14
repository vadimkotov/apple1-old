// #include <stdio.h>
#include <stdint.h>
#include <ncurses.h>
#include <string.h>

#include "IO.h"
#include "RAM.h"

IO::IO(RAM* ram) {
  this->ram = ram;

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
  scrollok(window, true);
}

IO::~IO() {
  endwin();
}


void IO::reset() {  
  // PA7 (i.e. bit 7 of KBD) is hardwired to +5V
  this->ram->write(PIA_KBD, 0x80);
  this->ram->write(PIA_KBD_CR, 0);
  this->ram->write(PIA_DSP, 0);
  this->ram->write(PIA_DSP_CR, 0);
}

void IO::update() {

  uint8_t dsp = this->ram->read(PIA_DSP);

  // If there's data to be displayed (b7)
  if (dsp & 0x80) {
    this->updateDisplay(dsp);

    /* 
       By this point whatever came from the 
       keyboard has been taken care of so
       clear b7 - no keyboard input
    */
    this->ram->write(PIA_KBD_CR, 0x27);
  }
  this->updateKeyboard();
  
}


void IO::updateDisplay(uint8_t dsp) {

    // Ignore b7 as it's a status bit
    dsp &= 0x7F;
    
    uint8_t charToDisplay = dsp;
    
    // Uppercase
    if (dsp >= 0x60 && dsp <= 0x7F)
      charToDisplay &= 0x5F;

    // Get the cursor position
    int x, y;
    getyx(this->window, y, x);
    
    if (charToDisplay == '\r') {
      move(++y, 0);      
    } else if (charToDisplay >= ' ' && charToDisplay <= '_') {
      addch(charToDisplay);
      ++x;
    } else if (!charToDisplay) {
      addch('@');
      // move(y, x);
      ++x;
    }
    
    refresh();
    
    // Vertical scrolling
    if (y == TEXT_HEIGHT) {
      scroll(this->window);
      move(--y, 0);
    }

    
    // Clear b7
    this->ram->write(PIA_DSP, dsp);
    
}


void IO::updateKeyboard() {
  
  char ch = getch();  

  if (ch != ERR) {
    // Uppercase
    if (ch >= 0x61 && ch <= 0x7a)
      ch &= 0x5F;

    if (ch == '\n')
      // Old ASCII keyboard...
      ch = '\r';

    this->ram->write(PIA_KBD, 0x80 | ch);
    // set b7 to 1 - character was read
    this->ram->write(PIA_KBD_CR, 0xA7);
  } 
}

