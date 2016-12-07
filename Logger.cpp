#include <stdio.h>
#include "Logger.h"
#include "CPU.h"
#include "IO.h"

Logger::Logger() {

  // this->kbd = 0;
  // this->dsp = 0;
  this->state = 0;
  
  this->file = fopen("execution-log", "w");

  char header[] = "| PC   | A  | X  | Y  | SP | NV-BDIZC | KBD | DSP |\n"
    "---------------------------------------------------\n";
  fwrite(header, 1, 104, this->file);
}

Logger::~Logger() {
  fclose(this->file);
}


void Logger::logCPU(CPU* cpu) {
  char buffer[256];
  uint8_t newKbd = cpu->ram->read(PIA_KBD);
  uint8_t newDsp = cpu->ram->read(PIA_DSP);

      
  sprintf(buffer, 
          "| %.4x | %.2x | %.2x | %.2x | %.2x "
          "| %d%d1%d%d%d%d%d | %.2x  | %.2x  |\n",
          cpu->PC, cpu->A, cpu->X, cpu->Y, cpu->SP, 
          cpu->negativeFlag, cpu->overflowFlag,
          cpu->breakCommand, cpu->decimalMode,
          cpu->interruptDisable, cpu->zeroFlag,
          cpu->carryFlag,
          newKbd, newDsp);

  
  fwrite(buffer, 1, 52, this->file);

}

void Logger::logStatus(CPU* cpu) {
  
  if (this->state == 0) {
    for (int i = 0; i < NUMBER_OF_FUNCTIONS; ++i) {
      if (cpu->PC == this->monitorAddresses[i]) {
        fwrite(this->monitorNames[i], 1, this->monitorSizes[i], this->file);
        fwrite(":\n", 1, 2, this->file);
        this->logCPU(cpu);
        this->state = 1;
      }
    }
    
  } else  {
    this->logCPU(cpu);
    this->state = 0;
  }

 

  // if (this->kbd != newKbd || this->dsp != newDsp) {

  // this->kbd = newKbd;
  // this->dsp = newDsp;

  
    //}
}

