#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#define HT_PRIME_1 151
#define HT_PRIME_2 163
#define HT_INITIAL_BASE_SIZE 53

typedef struct {
    char* key;
    char* value;
} ht_item;

typedef struct {
    int base_size;
    int size;
    int count;
    ht_item** items;
} ht_hash_table;

static ht_item HT_DELETED_ITEM = {NULL, NULL};

ht_hash_table* ht_new();
void ht_del_hash_table(ht_hash_table* ht);

void ht_insert(ht_hash_table* ht, const char* key, const char* value);
char* ht_search(ht_hash_table* ht, const char* key);
void ht_delete(ht_hash_table* ht, const char* key);
static void ht_resize_down(ht_hash_table* ht);
static void ht_resize_up(ht_hash_table* ht);

#endif
