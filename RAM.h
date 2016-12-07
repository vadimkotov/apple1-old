#pragma once
// #include <stdlib.h>
#include <stdint.h>

#define MEMORY_SIZE 0x10000

class RAM {
 private:
  uint8_t* memory;
  
 public:
  RAM();
  ~RAM();

  void load(uint16_t address, uint8_t* source, uint16_t size);
  void write(uint16_t address, uint8_t value);
  uint8_t read(uint16_t address);
};
