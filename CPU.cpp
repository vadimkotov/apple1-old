/*
  TODO:
  Count cycles
*/

// #include <stdio.h>
#include <stdint.h>

// #include <functional>
#include "CPU.h"


/*
  Decoding tables for opcodes
*/

mnem_id_t MnemDecodingTable[] = {
  /* 0*/ OP_BRK, OP_ORA, OP_NOP, OP_NOP, OP_NOP, OP_ORA, OP_ASL, OP_NOP, 
  /* 8*/ OP_PHP, OP_ORA, OP_ASL, OP_NOP, OP_NOP, OP_ORA, OP_ASL, OP_NOP, 
  /*10*/ OP_BPL, OP_ORA, OP_NOP, OP_NOP, OP_NOP, OP_ORA, OP_ASL, OP_NOP, 
  /*18*/ OP_CLC, OP_ORA, OP_NOP, OP_NOP, OP_NOP, OP_ORA, OP_ASL, OP_NOP, 
  /*20*/ OP_JSR, OP_AND, OP_NOP, OP_NOP, OP_BIT, OP_AND, OP_ROL, OP_NOP, 
  /*28*/ OP_PLP, OP_AND, OP_ROL, OP_NOP, OP_BIT, OP_AND, OP_ROL, OP_NOP, 
  /*30*/ OP_BMI, OP_AND, OP_NOP, OP_NOP, OP_NOP, OP_AND, OP_ROL, OP_NOP, 
  /*38*/ OP_SEC, OP_AND, OP_NOP, OP_NOP, OP_NOP, OP_AND, OP_ROL, OP_NOP, 
  /*40*/ OP_RTI, OP_EOR, OP_NOP, OP_NOP, OP_NOP, OP_EOR, OP_LSR, OP_NOP, 
  /*48*/ OP_PHA, OP_EOR, OP_LSR, OP_NOP, OP_JMP, OP_EOR, OP_LSR, OP_NOP, 
  /*50*/ OP_BVC, OP_EOR, OP_NOP, OP_NOP, OP_NOP, OP_EOR, OP_LSR, OP_NOP, 
  /*58*/ OP_CLI, OP_EOR, OP_NOP, OP_NOP, OP_NOP, OP_EOR, OP_LSR, OP_NOP, 
  /*60*/ OP_RTS, OP_ADC, OP_NOP, OP_NOP, OP_NOP, OP_ADC, OP_ROR, OP_NOP, 
  /*68*/ OP_PLA, OP_ADC, OP_ROR, OP_NOP, OP_JMP, OP_ADC, OP_ROR, OP_NOP, 
  /*70*/ OP_BVS, OP_ADC, OP_NOP, OP_NOP, OP_NOP, OP_ADC, OP_ROR, OP_NOP, 
  /*78*/ OP_SEI, OP_ADC, OP_NOP, OP_NOP, OP_NOP, OP_ADC, OP_ROR, OP_NOP, 
  /*80*/ OP_NOP, OP_STA, OP_NOP, OP_NOP, OP_STY, OP_STA, OP_STX, OP_NOP, 
  /*88*/ OP_DEY, OP_NOP, OP_TXA, OP_NOP, OP_STY, OP_STA, OP_STX, OP_NOP, 
  /*90*/ OP_BCC, OP_STA, OP_NOP, OP_NOP, OP_STY, OP_STA, OP_STX, OP_NOP, 
  /*98*/ OP_TYA, OP_STA, OP_TXS, OP_NOP, OP_NOP, OP_STA, OP_NOP, OP_NOP, 
  /*A0*/ OP_LDY, OP_LDA, OP_LDX, OP_NOP, OP_LDY, OP_LDA, OP_LDX, OP_NOP, 
  /*A8*/ OP_TAY, OP_LDA, OP_TAX, OP_NOP, OP_LDY, OP_LDA, OP_LDX, OP_NOP, 
  /*B0*/ OP_BCS, OP_LDA, OP_NOP, OP_NOP, OP_LDY, OP_LDA, OP_LDX, OP_NOP, 
  /*B8*/ OP_CLV, OP_LDA, OP_TSX, OP_NOP, OP_LDY, OP_LDA, OP_LDX, OP_NOP, 
  /*C0*/ OP_CPY, OP_CMP, OP_NOP, OP_NOP, OP_CPY, OP_CMP, OP_DEC, OP_NOP, 
  /*C8*/ OP_INY, OP_CMP, OP_DEX, OP_NOP, OP_CPY, OP_CMP, OP_DEC, OP_NOP, 
  /*D0*/ OP_BNE, OP_CMP, OP_NOP, OP_NOP, OP_NOP, OP_CMP, OP_DEC, OP_NOP, 
  /*D8*/ OP_CLD, OP_CMP, OP_NOP, OP_NOP, OP_NOP, OP_CMP, OP_DEC, OP_NOP, 
  /*E0*/ OP_CPX, OP_SBC, OP_NOP, OP_NOP, OP_CPX, OP_SBC, OP_INC, OP_NOP, 
  /*E8*/ OP_INX, OP_SBC, OP_NOP, OP_NOP, OP_CPX, OP_SBC, OP_INC, OP_NOP, 
  /*F0*/ OP_BEQ, OP_SBC, OP_NOP, OP_NOP, OP_NOP, OP_SBC, OP_INC, OP_NOP, 
  /*F8*/ OP_SED, OP_SBC, OP_NOP, OP_NOP, OP_NOP, OP_SBC, OP_INC, OP_NOP 
};


uint8_t CycleDecodingTable[] = {
  7, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 0, 4, 6, 0, 
  2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0, 
  6, 6, 0, 0, 3, 3, 5, 0, 4, 2, 2, 0, 4, 4, 6, 0, 
  2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0, 
  6, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 3, 4, 6, 0, 
  2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0, 
  6, 6, 0, 0, 0, 3, 5, 0, 4, 2, 2, 0, 5, 4, 6, 0, 
  2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0, 
  0, 6, 0, 0, 3, 3, 3, 0, 2, 0, 2, 0, 4, 4, 4, 0, 
  2, 6, 0, 0, 4, 4, 4, 0, 2, 5, 2, 0, 0, 5, 0, 0, 
  2, 6, 2, 0, 3, 3, 3, 0, 2, 2, 2, 0, 4, 4, 4, 0, 
  2, 5, 0, 0, 4, 4, 4, 0, 2, 4, 2, 0, 4, 4, 4, 0, 
  2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0, 
  2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0, 
  2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0, 
  2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0
};


addr_mode_t AddrModeDecodingTable[] = {
  IMP, IZX, IMP, IZX, ZPG, ZPG, ZPG, ZPG, 
  IMP, IMM, IMP, IMM, ABS, ABS, ABS, ABS, 
  REL, IZY, IMP, IZY, ZPX, ZPX, ZPX, ZPX, 
  IMP, ABY, IMP, ABY, ABX, ABX, ABX, ABX, 
  ABS, IZX, IMP, IZX, ZPG, ZPG, ZPG, ZPG, 
  IMP, IMM, IMP, IMM, ABS, ABS, ABS, ABS, 
  REL, IZY, IMP, IZY, ZPX, ZPX, ZPX, ZPX, 
  IMP, ABY, IMP, ABY, ABX, ABX, ABX, ABX, 
  IMP, IZX, IMP, IZX, ZPG, ZPG, ZPG, ZPG, 
  IMP, IMM, IMP, IMM, ABS, ABS, ABS, ABS, 
  REL, IZY, IMP, IZY, ZPX, ZPX, ZPX, ZPX, 
  IMP, ABY, IMP, ABY, ABX, ABX, ABX, ABX, 
  IMP, IZX, IMP, IZX, ZPG, ZPG, ZPG, ZPG, 
  IMP, IMM, IMP, IMM, IND, ABS, ABS, ABS, 
  REL, IZY, IMP, IZY, ZPX, ZPX, ZPX, ZPX, 
  IMP, ABY, IMP, ABY, ABX, ABX, ABX, ABX, 
  IMM, IZX, IMM, IZX, ZPG, ZPG, ZPG, ZPG, 
  IMP, IMM, IMP, IMM, ABS, ABS, ABS, ABS, 
  REL, IZY, IMP, IZY, ZPX, ZPX, ZPY, ZPY, 
  IMP, ABY, IMP, ABY, ABX, ABX, ABY, ABY, 
  IMM, IZX, IMM, IZX, ZPG, ZPG, ZPG, ZPG, 
  IMP, IMM, IMP, IMM, ABS, ABS, ABS, ABS, 
  REL, IZY, IMP, IZY, ZPX, ZPX, ZPY, ZPY, 
  IMP, ABY, IMP, ABY, ABX, ABX, ABY, ABY, 
  IMM, IZX, IMM, IZX, ZPG, ZPG, ZPG, ZPG, 
  IMP, IMM, IMP, IMM, ABS, ABS, ABS, ABS, 
  REL, IZY, IMP, IZY, ZPX, ZPX, ZPX, ZPX, 
  IMP, ABY, IMP, ABY, ABX, ABX, ABX, ABX, 
  IMM, IZX, IMM, IZX, ZPG, ZPG, ZPG, ZPG, 
  IMP, IMM, IMP, IMM, ABS, ABS, ABS, ABS, 
  REL, IZY, IMP, IZY, ZPX, ZPX, ZPX, ZPX, 
  IMP, ABY, IMP, ABY, ABX, ABX, ABX, ABX 
};



CPU::CPU(RAM* ram) {
  // Initialized the memory
  // this->memory.resize(MEMORY_SIZE);
  // this->memory = new uint8_t[MEMORY_SIZE];
  this->ram = ram;

  // TODO: Move that to init function
  // and make Reset call it
  // Initialize registers
    
  // Initialize cycle counter
  this->interruptPeriod = interruptPeriod;
  this->cycleCounter = interruptPeriod;
 
}


CPU::~CPU() {
  
}


void CPU::execute() {
  
  uint8_t opCode = this->ram->read(this->PC++);
  addr_mode_t addrMode = AddrModeDecodingTable[opCode];
  
  switch (MnemDecodingTable[opCode]) {
    // Arithmetic
  case OP_ADC:
    this->ADC(this->getOperandValue(addrMode));
    break;
  case OP_SBC:
    this->SBC(this->getOperandValue(addrMode));
    break;
  case OP_CMP:
    this->CMP(this->getOperandValue(addrMode));
    break;
  case OP_CPX:
    this->CPX(this->getOperandValue(addrMode));
    break;
  case OP_CPY:
    this->CPY(this->getOperandValue(addrMode));
    break;
    
    // Load / Store
  case OP_LDA:
    this->LDA(this->getOperandAddress(addrMode));
    break;
  case OP_LDX:
    this->LDX(this->getOperandAddress(addrMode));
    break;
  case OP_LDY:
    this->LDY(this->getOperandAddress(addrMode));
    break;
  case OP_STA:
    this->STA(this->getOperandAddress(addrMode));
    break;
  case OP_STX:
    this->STX(this->getOperandAddress(addrMode));
    break;
  case OP_STY:
    this->STY(this->getOperandAddress(addrMode));
    break;

    // Register transfer
  case OP_TAX:
    this->TAX();
    break;
  case OP_TAY:
    this->TAY();
    break;
  case OP_TXA:
    this->TXA();
    break;
  case OP_TYA:
    this->TYA();
    break;

    // Flag changes
  case OP_CLC:
    this->CLC();
    break;
  case OP_CLD:
    this->CLD();
    break;
  case OP_CLI:
    this->CLI();
    break;
  case OP_CLV:
    this->CLV();
    break;
  case OP_SEC:
    this->SEC();
    break;
  case OP_SED:
    this->SED();
    break;
  case OP_SEI:
    this->SEI();
    break;

    // Stack
  case OP_TSX:
    this->TSX();
    break;
  case OP_TXS:
    this->TXS();
    break;
  case OP_PHA:
    this->PHA();
    break;
  case OP_PHP:
    this->PHP();
    break;
  case OP_PLA:
    this->PLA();
    break;
  case OP_PLP:
    this->PLP();
    break;

    // Increments and decrements
  case OP_INC:
    this->INC(this->getOperandAddress(addrMode));
    break;
  case OP_INX:
    this->INX();
    break;
  case OP_INY:
    this->INY();
    break;
  case OP_DEC:
    this->DEC(this->getOperandAddress(addrMode));
    break;
  case OP_DEX:
    this->DEX();
    break;
  case OP_DEY:
    this->DEY();
    break;

    // Shits
  case OP_ASL:
    if (addrMode == IMP)
      this->ASL();
    else
      this->ASL(this->getOperandAddress(addrMode));
    break;
  case OP_LSR:
    if (addrMode == IMP)
      this->LSR();
    else
      this->LSR(this->getOperandAddress(addrMode));
    break;
  case OP_ROL:
    if (addrMode == IMP)
      this->ROL();
    else
      this->ROL(this->getOperandAddress(addrMode));
    break;
  case OP_ROR:
    if (addrMode == IMP)
      this->ROR();
    else
      this->ROR(this->getOperandAddress(addrMode));
    break;

    // Logic
  case OP_AND:
    this->AND(this->getOperandValue(addrMode));
    break;
  case OP_EOR:
    this->EOR(this->getOperandValue(addrMode));
    break;
  case OP_ORA:
    this->ORA(this->getOperandValue(addrMode));
    break;
  case OP_BIT:
    this->BIT(this->getOperandValue(addrMode));
    break;
    
    // Branches
  case OP_BCS:
    this->BCS((int8_t)this->getOperandValue(addrMode));
    break;
  case OP_BCC:
    this->BCC((int8_t)this->getOperandValue(addrMode));
    break;
  case OP_BEQ:
    this->BEQ((int8_t)this->getOperandValue(addrMode));
    break;
  case OP_BNE:
    this->BNE((int8_t)this->getOperandValue(addrMode));
    break;
  case OP_BMI:
    this->BMI((int8_t)this->getOperandValue(addrMode));
    break;
  case OP_BPL:
    this->BPL((int8_t)this->getOperandValue(addrMode));
    break;
  case OP_BVS:
    this->BVS((int8_t)this->getOperandValue(addrMode));
    break;    
  case OP_BVC:
    this->BVC((int8_t)this->getOperandValue(addrMode));
    break;

    // Jumps / Calls
  case OP_JMP:
    this->JMP(this->getOperandAddress(addrMode));
    break;
  case OP_JSR:
    this->JSR(this->getOperandAddress(addrMode));
    break;
  case OP_RTS:
    this->RTS();
    break;

    // Interrupts
  case OP_BRK:
    this->BRK();
    break;
  case OP_RTI:
    this->RTI();
    break;
    
    // NOP
  case OP_NOP:
    break;
    
    // Invalid
  default:
    // TODO: Throw exception
    break;
  }

}


void CPU::reset() {

  // Initialize registers
  this->X = 0;
  this->Y = 0;
  this->A = 0;

  // Initialize flags
  this->carryFlag = 0;
  this->zeroFlag = 0;
  this->interruptDisable = 0;
  this->decimalMode = 0;
  this->breakCommand = 0;
  this->overflowFlag = 0;
  this->negativeFlag = 0;
  
  // Stack poiner
  this->SP = STACK_START;

  this->PC = this->readWord(RESET_ISR);

  // Interrupts are disabled on reset
  this->interruptDisable = 1;
  
}

void CPU::NMI() {
  this->pushAddress(this->PC);
  // TODO: check how flags are pushed
  this->push(this->packFlags());
  this->PC = this->readWord(NMI_ISR);
  this->interruptDisable = 1;
  
}

void CPU::IRQ() {
  this->pushAddress(this->PC);
  /*
    IRQ clears the B flag in the
    saved status register
  */
  this->push(this->packFlags() & 0xEF);
  this->PC = this->readWord(IRQ_ISR);
  this->interruptDisable = 1;
  
}

/*
  Helper functions to deal with flags
*/
uint8_t CPU::packFlags() {
  uint8_t P = 0;
  P |= (this->negativeFlag << 7);
  P |= (this->overflowFlag << 6);
  P |= (1 << 5); // this is 'unused' flag and it's hardwired to 1
  P |= (this->breakCommand << 4);
  P |= (this->decimalMode << 3);
  P |= (this->interruptDisable << 2);
  P |= (this->zeroFlag << 1);
  P |= (this->carryFlag);
  return P;
}

void CPU::unpackFlags(uint8_t P) {
  this->negativeFlag = (P >> 7) & 1;
  this->overflowFlag = (P >> 6) & 1;
  this->breakCommand = (P >> 4) & 1;
  this->decimalMode = (P >> 3) & 1;
  this->interruptDisable = (P >> 2) & 1;
  this->zeroFlag = (P >> 1) & 1;
  this->carryFlag = P & 1;  
}

void CPU::updateOverflowFlag(uint8_t op1, uint8_t op2, uint8_t res) {
  this->overflowFlag = ((op1 ^ res) & (op2 ^ res) & 0x80) ? 1 : 0;
}

void CPU::updateNegativeFlag(uint8_t val) {
  this->negativeFlag = val >> 7; //(val & 0x80) ? 1 : 0;
}

void CPU::updateZeroFlag(uint8_t val) {
  this->zeroFlag = (!val) ? 1 : 0;
}

void CPU::updateZeroAndNegativeFlags(uint8_t val) {
  updateNegativeFlag(val);
  updateZeroFlag(val);
}

/*
  Helper functions for the  stack
*/

void CPU::push(uint8_t val) {
  this->ram->write(STACK_OFFSET + this->SP, val);
  this->SP--;
}

uint8_t CPU::pop() {
  this->SP++;
  return this->ram->read(STACK_OFFSET + this->SP);
}


void CPU::pushAddress(uint16_t address) {
  this->push(address >> 8);
  this->push(address & 0xFF);
}

uint16_t CPU::popAddress() {
  uint8_t adl = this->pop();
  uint8_t adh = this->pop();
  return (adh << 8) | adl;
}

/*
  Helper functions for reading / writing
  16-bit values
*/
uint16_t CPU::readWord(uint16_t address) {
  uint8_t adl = this->ram->read(address);
  uint8_t adh = this->ram->read(address + 1);
  return (adh << 8) | adl;
}

void CPU::writeWord(uint16_t address, uint16_t value) {
  this->ram->write(address, value & 0xFF);
  this->ram->write(address + 1, value >> 8);
}



/*
  Emulating reading / writing
  addresseses
*/


/*
  6502 OpCodes
*/

// Arithmetic
void CPU::ADC(uint8_t operand) {
  uint16_t sum16 = this->A + operand + this->carryFlag;
  uint8_t sum8 = (uint8_t)sum16;
  this->carryFlag = (sum16 > 0xFF) ? 1 : 0;
  this->updateOverflowFlag(this->A, operand, sum8);
  this->updateZeroAndNegativeFlags(sum8);
  this->A = sum8;
}


void CPU::SBC(uint8_t operand) {
  /*
    Subtraction with carry is essentially the same
    ass addition with carry with an operand in one's
    complement
  */
  this->ADC(255 - operand);  
}


void CPU::compare(uint8_t a, uint8_t b) {
  uint8_t dif = a - b;
  this->updateZeroAndNegativeFlags(dif);
  this->carryFlag = (a >= b) ? 1 : 0;
}

void CPU::CMP(uint8_t operand) {
  this->compare(this->A, operand);
}

void CPU::CPX(uint8_t operand) {
  this->compare(this->X, operand);
}

void CPU::CPY(uint8_t operand) {
  this->compare(this->Y, operand);
}

// Load Store
void CPU::load(uint8_t* target, uint16_t address) {
  *target = this->ram->read(address);
  this->updateZeroAndNegativeFlags(*target);
}

void CPU::LDA(uint16_t address) {
  this->load(&this->A, address);
}

void CPU::LDX(uint16_t address) {
  this->load(&this->X, address);
}

void CPU::LDY(uint16_t address) {
  this->load(&this->Y, address);
}

void CPU::STA(uint16_t address) {
  this->ram->write(address, this->A);
}

void CPU::STX(uint16_t address) {
  this->ram->write(address, this->X);
}

void CPU::STY(uint16_t address) {
  this->ram->write(address, this->Y);
}

// Register transfer
void CPU::TAX() {
  this->X = this->A;
  this->updateZeroAndNegativeFlags(this->X);
}

void CPU::TAY() {
  this->Y = this->A;
  this->updateZeroAndNegativeFlags(this->Y);
}

void CPU::TXA() {
  this->A = this->X;
  this->updateZeroAndNegativeFlags(this->A);
}

void CPU::TYA() {
  this->A = this->Y;
  this->updateZeroAndNegativeFlags(this->A);
}


// Flag changes
void CPU::CLC() {
  this->carryFlag = 0;
}

void CPU::CLD() {
  this->decimalMode = 0;
}

void CPU::CLI() {
  this->interruptDisable = 0;
}

void CPU::CLV() {
  this->overflowFlag = 0;
}

void CPU::SEC() {
  this->carryFlag = 1;
}

void CPU::SED() {
  this->decimalMode = 1;
}

void CPU::SEI() {
  this->interruptDisable = 1;
}

// Stack
void CPU::TSX() {
  this->X = this->SP;
  this->updateZeroAndNegativeFlags(this->X);
}

void CPU::TXS() {
  this->SP = this->X;
}

void CPU::PHA() {
  this->push(this->A);
}

void CPU::PHP() {
  this->push(this->packFlags());
}

void CPU::PLA() {
  this->A = this->pop();
  this->updateZeroAndNegativeFlags(this->A);
}

void CPU::PLP() {
  this->unpackFlags(this->pop());
}

// Increments and decrements
void CPU::INC(uint16_t address) {
  uint8_t val = this->ram->read(address) + 1;
  this->updateZeroAndNegativeFlags(val);
  this->ram->write(address,  val);
}

void CPU::INX() {
  this->X++;
  this->updateZeroAndNegativeFlags(this->X);
}

void CPU::INY() {
  this->Y++;
  this->updateZeroAndNegativeFlags(this->Y);
}

void CPU::DEC(uint16_t address) {
  uint8_t val = this->ram->read(address) - 1;
  this->updateZeroAndNegativeFlags(val);
  this->ram->write(address, val);
}

void CPU::DEX() {
  this->X--;
  this->updateZeroAndNegativeFlags(this->X);
}

void CPU::DEY() {
  this->Y--;
  this->updateZeroAndNegativeFlags(this->Y);
}


// Shifts
uint8_t CPU::doASL(uint8_t val) {
  this->carryFlag = (val >> 7);
  val <<= 1;
  this->updateZeroAndNegativeFlags(val);
  return val;
}

uint8_t CPU::doLSR(uint8_t val) {
  this->carryFlag = val & 1;
  val >>= 1;
  this->updateZeroAndNegativeFlags(val);
  return val;
}


uint8_t CPU::doROL(uint8_t val) {
  uint8_t carry = this->carryFlag;
  this->carryFlag = (val >> 7);
  val = (val << 1) | carry;
  this->updateZeroAndNegativeFlags(val);
  return val;
}


uint8_t CPU::doROR(uint8_t val) {
  uint8_t carry = this->carryFlag;
  this->carryFlag = (val & 1);
  val = (val >> 1) | (carry << 7);
  this->updateZeroAndNegativeFlags(val);
  return val;
}



void CPU::ASL() {
  SHIFT_INSTR_IMPLIED(doASL);
}

void CPU::ASL(uint16_t address) {
  SHIFT_INSTR_MEM_ACCESS(doASL);
}

void CPU::LSR() {
  SHIFT_INSTR_IMPLIED(doLSR);
}

void CPU::LSR(uint16_t address) {
  SHIFT_INSTR_MEM_ACCESS(doLSR);
}

void CPU::ROL() {
  SHIFT_INSTR_IMPLIED(doROL);
}

void CPU::ROL(uint16_t address) {
  SHIFT_INSTR_MEM_ACCESS(doROL);
}

void CPU::ROR() {
  SHIFT_INSTR_IMPLIED(doROR);
}

void CPU::ROR(uint16_t address) {
  SHIFT_INSTR_MEM_ACCESS(doROR);
}

// Logic
void CPU::AND(uint8_t operand) {
  this->A &= operand;
  this->updateZeroAndNegativeFlags(this->A);
}

void CPU::EOR(uint8_t operand) {
  this->A ^= operand;
  this->updateZeroAndNegativeFlags(this->A);
}

void CPU::ORA(uint8_t operand) {
  this->A |= operand;
  this->updateZeroAndNegativeFlags(this->A);
}

void CPU::BIT(uint8_t operand) {
  this->overflowFlag = (operand >> 6) & 1;
  this->updateZeroAndNegativeFlags(operand);
}

// Branches
void CPU::BCS(int8_t offset) {
  if (this->carryFlag)
    this->PC += offset;
}

void CPU::BCC(int8_t offset) {
  if (!this->carryFlag)
    this->PC += offset;
}

void CPU::BEQ(int8_t offset) {
  if (this->zeroFlag)
    this->PC += offset;
}

void CPU::BNE(int8_t offset) {
  if (!this->zeroFlag)
    this->PC += offset;
}

void CPU::BMI(int8_t offset) {
  if (this->negativeFlag)
    this->PC += offset;
}


void CPU::BPL(int8_t offset) {
  if (!this->negativeFlag)
    this->PC += offset;
}

void CPU::BVS(int8_t offset) {
  if (this->overflowFlag)
    this->PC += offset;
}

void CPU::BVC(int8_t offset) {
  if (!this->overflowFlag)
    this->PC += offset;
}

// Jumps / Calls
void CPU::JMP(uint16_t address) {
  this->PC = address;
}

void CPU::JSR(uint16_t address) {
  this->pushAddress(this->PC - 1);
  this->PC = address;
}

void CPU::RTS() {
  this->PC = this->popAddress() + 1; 
}

// Interrupts
void CPU::BRK() {
  this->pushAddress(this->PC);
  this->push(this->packFlags());
  this->PC = this->readWord(IRQ_ISR);
  this->breakCommand = 1;
}

void CPU::RTI() {
  // TODO: make sure flags are restored correctly
  this->unpackFlags(this->pop());
  this->PC = this->popAddress();
}


uint16_t CPU::getOperandAddress(addr_mode_t addrMode) {
  uint16_t address = 0;
  
  switch (addrMode) {
  case IMM: // Immediate
  case REL: // Relative (branching only)
    address = this->PC;
    this->PC += 1;
    break;

  case ZPG: // Zero Page
    address = this->ram->read(this->PC);
    this->PC += 1;
    break;

  case ZPX: // Zero Page, X 
    address = this->ram->read(this->PC) + this->X;
    this->PC += 1;
    break;

  case ZPY: // Zero Page, Y
    address = this->ram->read(this->PC) + this->Y;
    this->PC += 1;
    break;

  case ABS: // Absolute
    address = this->readWord(this->PC);
    this->PC += 2;
    break;

  case ABX: // Absolute, X
    address = this->readWord(this->PC) + this->X;
    this->PC += 2;
    break;

  case ABY: // Absolute, Y
    address = this->readWord(this->PC) + this->Y;
    this->PC += 2;
    break;

  case IZX: // (Zero Page, X)
    address = this->ram->read(this->PC);
    address = this->readWord(address + this->X);
    this->PC += 1;
    break;
    
  case IZY: // (Zero Page), Y
    address = this->ram->read(this->PC);
    address = this->readWord(address) + this->Y;
    this->PC += 1;
    break;
     
  case IMP: // Implied
    break;

  case IND: // Inderect (JMP only)
    address = this->readWord(this->PC);
    this->PC += 2;
    break;
    
  default:
    // Throw and error
    break;
  }
  return address;
}


uint8_t CPU::getOperandValue(addr_mode_t addrMode) {
  return this->ram->read(getOperandAddress(addrMode));
}



