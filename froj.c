#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parson.h"

//helpers
void _put(FILE *of, char const *str, char const *e) {
  size_t i = 0;
  size_t l = strlen(str);
  for (; i < l; ++i) fputc(str[i], of);

  i = 0;
  l = strlen(str);
  for (; i < l; i++) fputc(e[i], of);
  fputc('\n', of);
}

int convert_and_write(char* ifname) {
  JSON_Object *root = json_value_get_object(json_parse_file(ifname));
  if (!root) return 0;

  JSON_Array *functions = json_object_get_array(root, "functions");
  if (!functions) return 0;

  size_t funcount = json_array_get_count(functions);

  char ofname[strlen(ifname) + 6];
  strcpy(ofname, ifname);
  strcat(ofname, ".bril");
  FILE* of = fopen(ofname, "w");
  if(!of) return 0;

  JSON_Array *instrs, *args;
  JSON_Object *instr;
  char const *label, *op;

  for (size_t i = 0; i < funcount; ++i) {
    root = json_array_get_object(functions, i);
    _put(of, json_object_get_string(root, "name"), " {");
    instrs = json_object_get_array(root, "instrs");
    size_t instrcount = json_array_get_count(instrs); 
    for (size_t j = 0; j < instrcount; ++j) {
      instr = json_array_get_object(instrs, j);
      label = json_object_get_string(instr, "label");
      op = json_object_get_string(instr, "op");
      if (label) _put(of, label, ": ");
      else {
        fputs(op, of);
      }
    }
    fputs("}", of);
  }

  fclose(of);
  return 1;
}

int main (int argc, char* argv[]) {
  if (argc < 2) {
    puts("Must provide input file");
    return EXIT_FAILURE;
  }

  if (!convert_and_write(argv[1])) {
    puts("Error converting or writing, check file or debug");
    return EXIT_FAILURE;
  }
}
