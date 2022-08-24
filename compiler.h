#ifndef __COMPILER_H
#define __COMPILER_H

#include "ast.h"

void compile(Function *main);

#define ID_CHUNK	27		  /* binary files start with ESC... */
#define	SIGNATURE	"Lua"		/* ...followed by this signature */
#define	TEST_NUMBER	3.14159265358979323846E8

#endif // __COMPILER_H
