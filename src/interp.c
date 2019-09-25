#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../deps/parson.h"
#include "table.h"

#define jogs json_object_get_string
#define jogn json_object_get_number
#define jogb json_object_get_boolean
#define joga json_object_get_array
#define jags json_array_get_string
#define ARGS "args"
#define DEST "dest"
#define VALUE "value"

static JSON_Value *root = 0;
static JSON_Object *curr_instr = 0;
static table *mem = 0; 
static table *labels = 0;
static table *disp = 0;
static int64_t mem_args[2] = {0, 0};
static size_t ip = -1;
static size_t num_instrs = -1;
void (*op_funcs[19]) (void) = {0};

static void cleanup() {
  table_free(mem);
  table_free(labels);
  table_free(disp);
  json_value_free(root);
}


static void quit(char const *const err, char const *const extra) {
  puts(err);
  puts(extra);
  printf("instruction index: %zu\n", ip);
  cleanup();
  exit(EXIT_FAILURE);
}

static void get_or_quit() {
  bool not_found = false;
  JSON_Array *args = joga(curr_instr, "args");
  size_t num_args = json_array_get_count(args);
  for (size_t i = 0; i < num_args; ++i) {
    mem_args[i] = table_get(mem, jags(args, i), &not_found);
    if (not_found) {
      quit("Variable accessed before definition", jags(args, i));
    }
  }
}

static void put(int64_t val) {
  table_put(mem, jogs(curr_instr, DEST), val);
}


static void op_const() {
  const char *type = jogs(curr_instr, "type");
  if (!strcmp(type, "int"))
    table_put(mem, jogs(curr_instr, DEST), jogn(curr_instr, VALUE));
  else if (!strcmp(type, "bool")) 
    table_put(mem, jogs(curr_instr, DEST), (int64_t) jogb(curr_instr, VALUE));
  else
    quit("Unrecognized type", type);
}

static void op_add() {
  get_or_quit();
  put(mem_args[0]+mem_args[1]);
}

static void op_mul() {
  get_or_quit();
  put(mem_args[0]*mem_args[1]);
}

static void op_sub() {
  get_or_quit();
  put(mem_args[0] - mem_args[1]);
}

static void op_div() {
  get_or_quit();
  put(mem_args[0]/mem_args[1]);
}

static void op_eq() {
  get_or_quit();
  put(mem_args[0] == mem_args[1]);
}

static void op_lt() {
  get_or_quit();
  put(mem_args[0] < mem_args[1]);
}

static void op_gt() {
  get_or_quit();
  put(mem_args[0] > mem_args[1]);
}

static void op_le() {
  get_or_quit();
  put(mem_args[0] <= mem_args[1]);
}

static void op_ge() {
  get_or_quit();
  put(mem_args[0] >= mem_args[1]);
}

static void op_not() {
  get_or_quit();
  put(!mem_args[0]);
}

static void op_and() {
  get_or_quit();
  put(mem_args[0] & mem_args[1]);
}

static void op_or() {
  get_or_quit();
  put(mem_args[0] | mem_args[1]);
}

static void op_jmp() {
  bool not_found = false;
  char const *const label = jags(joga(curr_instr, ARGS), 0);
  size_t new_ip = (size_t) table_get(labels, label, &not_found);
  if (not_found) {
    quit("Label not in program", label);
  } else {
    ip = new_ip;
  }
}

static void op_br() {
  JSON_Array *args = joga(curr_instr, ARGS);
  bool not_found = false;
  bool branch_dne = false;
  char const *const cond_var = jags(args, 0);
  int64_t cond = table_get(mem, cond_var, &not_found);
  if (not_found) {
    quit("Used var before definition", cond_var);
  }
  char const *const tb = jags(args, 1);
  char const *const fb = jags(args, 2);
  int64_t tbip = table_get(labels, tb, &not_found);
  int64_t fbip = table_get(labels, fb, &branch_dne);
  if (not_found) quit("Label not found", tb);
  if (branch_dne) quit("Label not found", fb);
  if (cond) ip = tbip;
  else ip = fbip;
}

static void op_ret() {
  ip = num_instrs;
}

static void op_id() {
  get_or_quit();
  put(mem_args[0]);
}

static void op_print() {
  bool not_found = false;
  JSON_Array *args = json_object_get_array(curr_instr, ARGS);
  size_t num_args = json_array_get_count(args);
  for (size_t i = 0; i < num_args; ++i) {
    int64_t val = table_get(mem, jags(args, i), &not_found);
    if (not_found) {
      printf("Error: %s used before definition", jags(args, i));
      return;
    } else {
      printf("%" PRId64 " ", val);
    }
  }
  puts("");
}

static void op_nop() {
  return;
}

static void make_dispatch() {
  char *ops[19] = {
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
  disp = table_make(19, ops);
  op_funcs[0]  = &op_const;
  op_funcs[1]  = &op_add;
  op_funcs[2]  = &op_mul;
  op_funcs[3]  = &op_sub;
  op_funcs[4]  = &op_div;
  op_funcs[5]  = &op_eq;
  op_funcs[6]  = &op_lt;
  op_funcs[7]  = &op_gt;
  op_funcs[8]  = &op_le;
  op_funcs[9]  = &op_ge;
  op_funcs[10] = &op_not;
  op_funcs[11] = &op_and;
  op_funcs[12] = &op_or;
  op_funcs[13] = &op_jmp;
  op_funcs[14] = &op_br;
  op_funcs[15] = &op_ret;
  op_funcs[16] = &op_id;
  op_funcs[17] = &op_print;
  op_funcs[18] = &op_nop;
}

static JSON_Array *get_main_func_instrs(char const *const fn) {
  JSON_Object *root_obj = 0;
  JSON_Array *funcs = 0;
  JSON_Object *almost_there = 0;
  JSON_Array *instrs = 0;

  root = json_parse_file(fn);
  root_obj = json_value_get_object(root);
  funcs = json_object_get_array(root_obj, "functions");
  almost_there = json_array_get_object(funcs, 0);
  instrs = json_object_get_array(almost_there, "instrs");

  return instrs;
}

/*
 * calling function checks f is valid file
 */

uint64_t interp(char const *const fn) {
  JSON_Array *instrs = get_main_func_instrs(fn);
  struct timespec tick, tock;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tick);
  mem = table_init();
  labels = table_init();
  make_dispatch();

  num_instrs = json_array_get_count(instrs);
  
  for (size_t i = 0; i < num_instrs; ++i) {
    curr_instr = json_array_get_object(instrs, i);
    const char *label = json_object_get_string(curr_instr, "label");
    if (label) {
      table_put(labels, label, i);
    }
  }

  size_t op_ind = -1;
  char const *op = 0;
  bool not_found = false;
  for (ip = 0; ip < num_instrs; ++ip) {
    curr_instr = json_array_get_object(instrs, ip);
    op = jogs(curr_instr, "op");
    if (!op) continue; // label
    op_ind = (size_t) table_get(disp, op, &not_found);
    if (not_found) {
      quit("Operation not defined", op);
    } else {
      (op_funcs[op_ind]());
    }
  }

  cleanup();
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tock);
  return (1000000000 * (tock.tv_sec - tick.tv_sec) + tock.tv_nsec - tick.tv_nsec);
}
