#include <dirent.h>
#include <inttypes.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define NUM_RUNS 10000

extern uint64_t interp(char const *const);

int main(int argc, char **argv) {
  if (argc < 2) {
    puts("Must supply bril file");
    exit(EXIT_FAILURE);
  }
  char *fn = argv[1];
  if (!strcmp(fn, "--benchmark")) {
    long double times[NUM_RUNS] = {0};
    char *dirs[3] = {"test/interp/", "test/parse/", "test/print/"};
    DIR* dir;
    struct dirent *dent;
    printf("NUM RUNS: %d\n", NUM_RUNS);
    for (size_t i = 0; i < 3; ++i) {
      dir = opendir(dirs[i]);
      if (!dir) {
        printf("Can't open %s directory\n", dirs[i]);
        continue;
      }
      char full_path[100] = {0};
      puts(dirs[i]);
      puts("--------------------------------------");
      while ((dent = readdir(dir))) {
        if (strstr(dent->d_name, ".json")) {
          long double time_milli = 0.0;
          long double avg_time_milli = 0.0;
          long double sum = 0.0;
          long double sigma = 0.0;

          strcpy(full_path, dirs[i]);
          strcat(full_path, dent->d_name);
          puts(full_path);
          for (size_t j = 0; j < NUM_RUNS; ++j) {
            time_milli = ((long double) interp(full_path))/1e6;
            sum += time_milli;
            times[j] = time_milli;
          }
          avg_time_milli = sum/NUM_RUNS;
          sum = 0.0;
          for (size_t j = 0; j < NUM_RUNS;++j) {
            sum += pow(times[j] - avg_time_milli, 2);
          }
          sigma = sqrt(sum/NUM_RUNS);
          printf("MEAN: %Lf\n", avg_time_milli);
          printf("STDV: %Lf\n\n", sigma);
        }
      }
    }
    closedir(dir);
  } else {
    FILE* f = fopen(fn, "r");
    if (!f) {
      printf("Error opening %s\n", fn);
      exit(EXIT_FAILURE);
    }
    fclose(f);
    uint64_t time_spent = interp(fn);
    printf("%" PRIu64 " nsecs\n", time_spent);
  }
  return 0;
}
