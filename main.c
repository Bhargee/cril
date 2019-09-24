#include <stdlib.h>
#include <stdio.h>

extern void interp(char const *const);

int main(int argc, char **argv) {
  if (argc < 2) {
    puts("Must supply bril file");
    exit(EXIT_FAILURE);
  }
  char *fn = argv[1];
  FILE* f = fopen(fn, "r");
  if (!f) {
    printf("Error opening %s\n", fn);
    exit(EXIT_FAILURE);
  }
  fclose(f);
  interp(fn);
  return 0;
}
