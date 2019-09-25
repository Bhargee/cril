#include <stdlib.h>
#include <stdio.h>

extern double interp(char const *const);

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
  double time_spent = interp(fn);
  printf("%f secs\n", time_spent);
  return 0;
}
