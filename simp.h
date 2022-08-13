#include <stdint.h>
#include <vector>

#define ID_CHUNK	27		  /* binary files start with ESC... */
#define	SIGNATURE	"Lua"		/* ...followed by this signature */

#define SIZE_INSTRUCTION        32 /* instruction is 32 bits wide */
#define SIZE_B          9       /* B argument is 9 bits wide */
#define SIZE_OP         6       /* opcode is 6 bits wide */

#define	TEST_NUMBER	3.14159265358979323846E8

typedef uint8_t byte;

// ast node types
typedef enum {
  NODE_NUM,
  NODE_BINOP,
  NODE_UNOP,
  NODE_CALL,
  NODE_FUNC,
  NODE_RETURN,
  NODE_IF,
  NODE_WHILE,
  NODE_READ,
  NODE_WRITE,
  NODE_ASSIGN,
  NODE_ID,
} node_type;

// ast tree node
typedef struct {
  node_type type;
  char* name;
  int line;
  int col;
  int val;
  vector children;
} AstNode;

typedef struct {
  char* name;
  int start_pc;
  int end_pc;
} Local;

/*
** Function Prototypes
*/
typedef struct {
  char* source_name;
  int line_defined;
  int num_params;
  byte is_vararg;
  int max_stack;

  list statements;
  list locals;
  list lines;

  /* constants */
  list knum;
  list kstr;
  list functions;

  list code;
} Function;

