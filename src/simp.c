#include "compiler.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"

int main(int argc, char *argv[]) {
  FILE *infile;
  FILE *outfile = NULL;

  if (strcmp(argv[1], "-o") == 0) {
    outfile = fopen(argv[2], "w");

    if (outfile == NULL) {
      fprintf(stderr, "Could not open output file %s\n", argv[2]);
      return 1;
    }

    argv += 2;
  }

  infile = fopen(argv[1], "r");

  if (infile == NULL) {
    fprintf(stderr, "Could not open file\n");
    exit(EXIT_FAILURE);
  }

  Function *f = parse(infile);

  char chunk[2048];
  int size = compile(f, chunk);

  if(has_semantic_warnings()) {
    display_warning_list();
  }

  if(has_semantic_errors()) {
    display_error_list();
    exit(EXIT_FAILURE);
  }

  if (outfile != NULL) {
    fwrite(chunk, 1, size, outfile);
  } else {
    lua_State *L = lua_open(0);
    lua_baselibopen(L);
    lua_iolibopen(L);
    lua_dobuffer(L, chunk, size, argv[1]);
    lua_close(L);
  }

  return 0;
}
