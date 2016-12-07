#include <stdio.h>
#include <stdlib.h>
#include "RAM.h"
#include "CPU.h"
#include "IO.h"
#include "Logger.h"

// TODO: sort include guards

/*
  [   Apple I Memory Layout   ]
  -----------------------------
  | $0000 - $7FFF RAM         |
  -----------------------------
  | $8000 - $BFFF Unused      |
  | $C000 - $CFFF Casette     |
  | $D000 - $DFFF PIA         |
  | $E000 - $EFFF BASIC       |
  | $F000 - $FFFF ASM & WozMon|
  -----------------------------
*/


#define ROM_PATH    "eeprom.hex"
#define ROM_ADDRESS 0x8000


size_t readFile(const char* path, uint8_t** buffer) {
  FILE* pf = fopen(path, "rb");
  if (pf == NULL) {
    printf("[ERROR]= Couldn't open file %s\n", path);
    exit(1);
  }

  fseek(pf, 0, SEEK_END);
  size_t size = ftell(pf);
  rewind(pf);

  *buffer = new uint8_t[size];
  if (*buffer == NULL) {
    printf("[ERROR] Couldn't allocate buffer\n");
    exit(1);
  }

  size_t res = fread(*buffer, 1, size, pf);

  if (res != size) {
    printf("[ERROR] Couldn't read the file\n");
    exit(1);
  }

  fclose(pf);
  return size;
}

void printCPUStatus(CPU* cpu) {
  // printf("\n[CPU STATUS]\n");
  printf(" A $%.2x | ", cpu->A);
  printf(" X $%.2x | ", cpu->X);
  printf(" Y $%.2x\n", cpu->Y);
  printf("SP $%.2x | ", cpu->SP);
  printf("PC $%.4x\n", cpu->PC);
  printf("NV-BDIZC\n");
  printf("%d%d1%d%d%d%d%d\n\n", cpu->negativeFlag,
         cpu->overflowFlag, cpu->breakCommand,
         cpu->decimalMode, cpu->interruptDisable,
         cpu->zeroFlag, cpu->carryFlag);
}



int main(int argc, char* argv[]) {

  RAM* ram = new RAM();
  CPU* cpu = new CPU(ram);
  IO* io = new IO(ram);

  uint8_t* program;
  size_t size = readFile(ROM_PATH, &program);

  ram->load(ROM_ADDRESS, program, size);
  
  cpu->reset();
  io->reset();

  Logger *logger = new Logger();
  
  // for (int i = 0; i < 20; i++) {
  for (;;) {
    logger->logStatus(cpu);
    cpu->execute();
    io->update();
  }
  

  delete io;
  delete cpu;
  delete ram;
  return 0;
}
