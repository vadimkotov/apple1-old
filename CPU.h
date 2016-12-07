#pragma once

#include <stdint.h>
#include <functional>
#include "RAM.h"

// #define MNEM_TOTAL 56

typedef enum {
  OP_ADC, OP_AND, OP_ASL, OP_BCC, OP_BCS, OP_BEQ, OP_BIT, OP_BMI,
  OP_BNE, OP_BPL, OP_BRK, OP_BVC, OP_BVS, OP_CLC, OP_CLD, OP_CLI,
  OP_CLV, OP_CMP, OP_CPX, OP_CPY, OP_DEC, OP_DEX, OP_DEY, OP_EOR,
  OP_INC, OP_INX, OP_INY, OP_JMP, OP_JSR, OP_LDA, OP_LDX, OP_LDY,
  OP_LSR, OP_NOP, OP_ORA, OP_PHA, OP_PHP, OP_PLA, OP_PLP, OP_ROL,
  OP_ROR, OP_RTI, OP_RTS, OP_SBC, OP_SEC, OP_SED, OP_SEI, OP_STA,
  OP_STX, OP_STY, OP_TAX, OP_TAY, OP_TSX, OP_TXA, OP_TXS, OP_TYA
} mnem_id_t;


typedef enum {
  IMM, // Immediate: ADC #5
  ZPG, // Zero Page: ADC $10
  ZPX, // Zero Page, X: ADC $10, X
  ZPY, // Zero Page, Y: 
  ABS, // Absolute: ADC $1010
  ABX, // Absolute, X: ADC $1010, X
  ABY, // Absolute, Y: ADC $1010, Y
  IZX, // (Zero Page, X): ADC ($10, X)
  IZY, // (Zero Page), Y: ADC ($10), Y
  REL, // Relative, branching only
  IMP, // Implied
  IND  // Indirect (JMP only) 
} addr_mode_t;


/*
  Remember that stack grows
  downwards
*/
#define STACK_OFFSET 0x0100
#define STACK_START  0xFF

/*
  Interrupts handlers a.k.a. interrupt
  service routines (ISR)
*/

#define NMI_ISR   0xFFFA
#define RESET_ISR 0xFFFC
#define IRQ_ISR   0xFFFE



class CPU {
 public:
  CPU(RAM* ram);
  ~CPU();

  void execute();
  void reset();
  void NMI();
  void IRQ();


  RAM* ram;
  
  /*
    Since address bus is 16 bits the program
    counter is 16 bits too, cause it points
    to the address of the next instruction 
  */

  uint16_t PC;

  // Stack pointer is also 8 bit
  // it's relative to 0x100
  uint8_t SP;
  
  // Accumulator
  uint8_t A;

  // Index registers
  uint8_t X;
  uint8_t Y;
  /*
    Status register (P) breakdown
    -------------
    Bit | Meaning
    -------------
     0  | Carry flag
     1  | Zero flag
     2  | Interrupt disable
     3  | Decimal mode
     4  | Break command
     5  | Not used ?
     6  | Overflow flag
     7  | Negative flag
  */
  // uint8_t P;
  uint8_t carryFlag;
  uint8_t zeroFlag;
  uint8_t interruptDisable;
  uint8_t decimalMode;
  uint8_t breakCommand;
  uint8_t overflowFlag;
  uint8_t negativeFlag;
  
  /*
    We need to keep the cycle counter so that
    all our emulated devices are synced
  */

  uint32_t cycleCounter;
  
  
 private:
  
  /*
    In different devices the CPU is clocked with
    a different frequency, e.g. for Apple I it's 1 MHz,
    for NES ~ 1.79 MHz
  */

  uint32_t interruptPeriod;

  /*
    Helper functions to deal with flags
  */
  uint8_t packFlags();
  void unpackFlags(uint8_t P);
  void updateOverflowFlag(uint8_t op1, uint8_t op2, uint8_t result);
  void updateNegativeFlag(uint8_t val);
  void updateZeroFlag(uint8_t val);
  void updateZeroAndNegativeFlags(uint8_t val);

  /*
    Helper functions for the stack
  */
  void push(uint8_t value);
  uint8_t pop();  
  void pushAddress(uint16_t address);
  uint16_t popAddress();
  
  /*
    Helper functions for reading / writing
    16-bit values
  */
  uint16_t readWord(uint16_t address);
  void writeWord(uint16_t address, uint16_t value);
  
  /*
    6502 OpCodes
  */
  // Arithmetic (2)
  void ADC(uint8_t operand);
  void SBC(uint8_t operand);

  void compare(uint8_t a, uint8_t b);
  void CMP(uint8_t operand);
  void CPX(uint8_t operand);
  void CPY(uint8_t operand);

  // Load / Store (6)
  void load(uint8_t* target, uint16_t address);
  void LDA(uint16_t address);
  void LDX(uint16_t address);
  void LDY(uint16_t address);

  void store(uint8_t* target, uint16_t address);
  void STA(uint16_t address);
  void STX(uint16_t address);
  void STY(uint16_t address);

  // Register transfer (4)
  void TAX();
  void TAY();
  void TXA();
  void TYA();

  // Flag changes (7)
  void CLC();
  void CLD();
  void CLI();
  void CLV();
  void SEC();
  void SED();
  void SEI();

  // Stack (6)
  void TSX();
  void TXS();
  void PHA();
  void PHP();
  void PLA();
  void PLP();

  // Increments and decrements (6)
  void INC(uint16_t address);
  void INX();
  void INY();
  void DEC(uint16_t address);
  void DEX();
  void DEY();

  // Shifts  (4)
  
  /*
    I know it's not consistent that this is the only
    type of instruction that requires overloading, 
    and some macros, but I can't think about anything 
    better from the reading / maintainablity point of view.
    We have a function with no arguments for implied
    addressing mode and address otherwise
  */


#define SHIFT_INSTR_IMPLIED(Func) {             \
    this->A = this->Func(this->A);              \
  }
  
#define SHIFT_INSTR_MEM_ACCESS(Func) {                          \
    uint8_t val = this->ram->read(address);                     \
    val = this->Func(val);                                      \
    this->ram->write(address, val);                             \
  }
  
  uint8_t doASL(uint8_t val);
  uint8_t doLSR(uint8_t val);
  uint8_t doROL(uint8_t val);
  uint8_t doROR(uint8_t val);
  void ASL();
  void ASL(uint16_t address);
  void LSR();
  void LSR(uint16_t address);
  void ROL();
  void ROL(uint16_t address);
  void ROR();
  void ROR(uint16_t address);

  // Logic (4)
  void AND(uint8_t operand);
  void EOR(uint8_t operand);
  void ORA(uint8_t operand);
  void BIT(uint8_t operand);

  // Branches
  void BCS(int8_t offset);
  void BCC(int8_t offset);
  void BEQ(int8_t offset);
  void BNE(int8_t offset);
  void BMI(int8_t offset);
  void BPL(int8_t offset);
  void BVS(int8_t offset);
  void BVC(int8_t offset);

  
  // Jumps / Calls
  void JMP(uint16_t address);
  void JSR(uint16_t address);
  void RTS();

  // Interrupts
  void BRK();
  void RTI();

  // NOP
  // void NOP();
  
  /* 
     Functions to help
     with addressing modes 
  */
  uint16_t getOperandAddress(addr_mode_t addrMode);
  uint8_t getOperandValue(addr_mode_t addrMode); 
 
};
