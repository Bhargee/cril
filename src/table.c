/* TODO
 * use `realloc` instead of malloc + free combos
 * minimize use of mallocs? Or use c89 standard
 */
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "table.h"

#define TABLE_INIT_SIZE 50
#define LOAD_FACTOR .48

int64_t _hash(char const *const key) {
  int n = strlen(key);
  int64_t h = key[n--];
  for (; n >= 0; --n) {
    h = (31*h) + key[n];
  }
  return h;
}

table *table_init() {
  table *t = (table *) malloc(sizeof (table));
  t->size = TABLE_INIT_SIZE;
  t->num_elems = 0;
  t->storage = (table_elem*) calloc(TABLE_INIT_SIZE,  sizeof(table_elem));

  return t;
}


int64_t table_get(table* t, char const *const key, bool *not_found) {
  *not_found = false;
  size_t i = _hash(key) % t->size;
  for (; i < t->size; i++) {
    if (!t->storage[i].key) {
      *not_found = true;
       return 0;
    }
    else if(!strcmp(t->storage[i].key, key)) {
      return t->storage[i].value;
    }
  }
  if (i == t->size) {
    *not_found = true;
    return 0;
  }
}

void table_put(table* t, char const *const key, int64_t value) {
  if ((((float) t->num_elems)/t->size)+.01 >= LOAD_FACTOR) {
    size_t new_size = 2*t->size;
    table_elem *new_storage = calloc(new_size, sizeof(table_elem));
    for (size_t i = 0; i < t->size; i++) { 
      if (t->storage[i].key) {
        size_t new_index = _hash(t->storage[i].key) % new_size;
        for (; new_index < new_size; ++new_index) {
          if (!new_storage[new_index].key) {
            new_storage[new_index] = t->storage[i];
            break;
          }
        }
      }
    }
    free(t->storage);
    t->storage = new_storage;
    t->size = new_size;
  }

  for(size_t i = _hash(key) % t->size; i < t->size; ++i) {
    bool empty_key = !t->storage[i].key;
    bool same_key = !empty_key && !strcmp(t->storage[i].key, key);
    if (same_key) {
      t->storage[i].value = value;
      return;
    }
    else if (empty_key) {
      t->storage[i].value = value;
      t->storage[i].key = (char*) malloc(strlen(key)+1);
      strcpy(t->storage[i].key, key);
      t->num_elems++;
      return;
    }
  }
}

table* table_make(size_t arr_len, char **strs) {
  table *t = (table *) malloc(sizeof (table));
  size_t init_size = TABLE_INIT_SIZE;
  while (init_size < arr_len) {
    init_size += TABLE_INIT_SIZE;
  }
  t->size = init_size;
  t->num_elems = arr_len;
  t->storage = (table_elem*) calloc(TABLE_INIT_SIZE,  sizeof(table_elem));
  for (size_t i = 0; i < arr_len; ++i) {
    table_put(t, strs[i], i);
  }

  return t;
}



void table_free(table *t) {
  for (size_t i = 0; i < t->size; ++i) {
    free(t->storage[i].key);
  }
  free(t->storage);
  free(t);
}

void table_print(table *t) {
  for (size_t i = 0; i < t->size; ++i) {
    if (t->storage[i].key) {
      printf("key: %s, value: %d, index: %zu\n", t->storage[i].key, (int) t->storage[i].value, i);
    }
  }
}
