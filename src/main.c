#include <inttypes.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

extern uint64_t interp(char const *const);

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
  uint64_t time_spent = interp(fn);
  printf("%" PRIu64 " nsecs\n", time_spent);
  return 0;
}
