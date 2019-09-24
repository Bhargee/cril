/*
 * TODO
 * vararg the JSON_* deletions
 * add general program error handling?
 */
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "parson.h"
#include "table.h"

#define jogs json_object_get_string
#define jogi json_object_get_number
#define jags json_array_get_string
#define ARGS "args"
#define DEST "dest"
#define VALUE "value"

static JSON_Value *_root = 0;
static table *_mem = 0; 
static table *_labels = 0;

static char *_ops[19] = {
  "const",
  "add",
  "mul",
  "sub",
  "div",
  "eq",
  "lt",
  "gt",
  "le",
  "ge",
  "not",
  "and",
  "or",
  "jmp",
  "br",
  "ret",
  "id",
  "print",
  "nop"
};

static void _op_const(JSON_Object *instr) {
 table_add(_mem, jogs(instr, DEST), jogi(instr, VALUE));
}

static void _op_print(JSON_Object *instr) {
  bool not_found = false;
  JSON_Array *args = json_object_get_array(instr, ARGS);
  size_t num_args = json_array_get_count(args);
  for (size_t i = 0; i < num_args; ++i) {
    int64_t val = table_get(_mem, jags(args, i), &not_found);
    if (not_found) {
      printf("Error: %s used before definition", jags(args, i));
      return;
    } else {
      printf("%" PRId64 " ", val);
    }
  }
  puts("");
}


static void _op_nop(JSON_Object *instr) {
  return;
}

void (*_op_funcs[19]) (JSON_Object *instr) = {
  [0] = _op_const, 
  [17] = _op_print, 
  [18] = _op_nop
};

size_t _instrs_to_op(JSON_Object *instr) {
  const char *op = json_object_get_string(instr, "op");
  // TODO see if you can do this by parameterizing num ops
  for (size_t i = 0; i < 19; ++i) {
    if (!strcmp(_ops[i], op)) return i;
  }
}

JSON_Array *_get_main_func_instrs(char const *const fn) {
  JSON_Object *root_obj = 0;
  JSON_Array *funcs = 0;
  JSON_Object *almost_there = 0;
  JSON_Array *instrs = 0;

  _root = json_parse_file(fn);
  root_obj = json_value_get_object(_root);
  funcs = json_object_get_array(root_obj, "functions");
  almost_there = json_array_get_object(funcs, 0);
  instrs = json_object_get_array(almost_there, "instrs");

  return instrs;
}

/*
 * calling function checks f is valid file
 */

void interp(char const *const fn) {
  _mem = table_init();
  _labels = table_init();

  JSON_Object *curr_instr = 0;
  JSON_Array *instrs = _get_main_func_instrs(fn);
  size_t num_instrs = json_array_get_count(instrs);
  
  for (size_t i = 0; i < num_instrs; ++i) {
    curr_instr = json_array_get_object(instrs, i);
    const char *label = json_object_get_string(curr_instr, "label");
    if (label) {
      table_add(_labels, label, i);
    }
  }

  for (size_t i = 0; i < num_instrs; ++i) {
    curr_instr = json_array_get_object(instrs, i);
    (_op_funcs[_instrs_to_op(curr_instr)])(curr_instr);
  }

  json_value_free(_root);
  table_free(_mem);
  table_free(_labels);
}
