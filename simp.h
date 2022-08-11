#include <stdint.h>

#include "list.h"

#define ID_CHUNK	27		  /* binary files start with ESC... */
#define	SIGNATURE	"Lua"		/* ...followed by this signature */

#define SIZE_INSTRUCTION        32 /* instruction is 32 bits wide */
#define SIZE_B          9       /* B argument is 9 bits wide */
#define SIZE_OP         6       /* opcode is 6 bits wide */

#define	TEST_NUMBER	3.14159265358979323846E8

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

