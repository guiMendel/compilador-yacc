#include <stdint.h>

#define SIGNATURE 0x1b4c7561

typedef uint8_t byte;

typedef struct {
  char* name;
  int start_pc;
  int end_pc;
} Local;

typedef struct {
  char* source_name;
  int line_defined;
  int num_params;
  byte is_vararg;
  int max_stack;
  list locals;
  list lines;
  list constants;
  list code;
} Function;

