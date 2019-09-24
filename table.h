#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct table_elem {
  char *key;
  int64_t value;
} table_elem;

typedef struct table {
  size_t size;
  size_t num_elems;
  table_elem* storage;
} table;

table *table_init();
table* table_make(size_t, char**);
int64_t table_get(table*, char const *const, bool*);
void table_put(table*, char const *const, int64_t);
void table_free(table*);
void table_print(table*);
