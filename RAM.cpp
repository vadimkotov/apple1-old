#include <stdint.h>
#include <cstring>
#include "RAM.h"

RAM::RAM() {
  this->memory = new uint8_t[MEMORY_SIZE];
}

RAM::~RAM() {
  delete memory;
}

void RAM::load(uint16_t address, uint8_t* source, uint16_t size) {
  if (address + size >= MEMORY_SIZE) {
    // TODO trow exception
  }
  std::memcpy(&this->memory[address], source, size);
}

void RAM::write(uint16_t address, uint8_t value) {
  this->memory[address] = value;
}

uint8_t RAM::read(uint16_t address) {
  return this->memory[address];
}
