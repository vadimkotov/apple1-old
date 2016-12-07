#include <stdio.h>
#include <stdint.h>
#include "CPU.h"

/* #define RESET 0xFF00 */
/* #define NOTCR 0xFF0F */
/* #define */
#define NUMBER_OF_FUNCTIONS 25

class Logger {
 public:
  Logger();
  ~Logger();
  void logStatus(CPU* cpu);
  
 private:
  FILE* file;

  // uint8_t kbd;
  // uint8_t dsp;
  uint8_t state;
  
  const char* monitorNames[NUMBER_OF_FUNCTIONS] = {
    "RESET", "NOTCR", "ESCAPE", "GETLINE", "BACKSPACE",
    "NEXTCHAR", "SETSTOR", "SETMODE", "BLSKIP", "NEXT ITEM",
    "NEXTHEX", "DIG", "HEXSHIFT", "NOTHEX", "TONEXTITEM",
    "RUN", "NOTSTOR", "SETADR", "NXTPRNT", "PRDATA",
    "XAMNEXT", "MOD8CHK", "PRBYTE", "PRHEX", "ECHO"
  };

  const int monitorSizes[NUMBER_OF_FUNCTIONS] = {
    5, 5, 6, 7, 9,
    8, 7, 7, 6, 9,
    7, 3, 8, 6, 10,
    3, 7, 6, 7, 6,
    7, 7, 6, 5, 4
  };

  const uint16_t monitorAddresses[NUMBER_OF_FUNCTIONS] = {
    0xFF00, 0xFF0F, 0xFF1A, 0xFF1F, 0xFF26,
    0xFF29, 0xFF40, 0xFF41, 0xFF43, 0xFF44,
    0xFF5F, 0xFF6E, 0xFF74, 0xFF7F, 0xFF91,
    0xFF94, 0xFF97, 0xFF9B, 0xFFA4, 0xFFBA,
    0xFFC4, 0xFFD6, 0xFFDC, 0xFFE5, 0xFFEF
  };


  void logCPU(CPU* cpu);
};
