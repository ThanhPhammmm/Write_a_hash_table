#include "hash_table.h"
#include "prime.h"

static ht_item* ht_new_item(const char* k, const char* v) {
    ht_item* i = malloc(sizeof(ht_item));
    i->key = strdup(k);
    i->value = strdup(v);
    return i;
}

static void ht_del_item(ht_item* i) {
    free(i->key);
    free(i->value);
    free(i);
}

static void* xmalloc(size_t size) {
    void* p = malloc(size);
    if (!p) {
        fprintf(stderr, "malloc failed\n");
        exit(EXIT_FAILURE);
    }
    return p;
}

static void* xcalloc(size_t n, size_t s) {
    void* p = calloc(n, s);
    if (!p) {
        fprintf(stderr, "calloc failed\n");
        exit(EXIT_FAILURE);
    }
    return p;
}

/* ---------- HASH FUNCTIONS ---------- */
static int ht_hash(const char* s, const int a, const int m) {
    long hash = 0;
    const int len_s = strlen(s);
    for (int i = 0; i < len_s; i++) {
        hash += (long)pow(a, len_s - (i + 1)) * s[i];
        hash %= m;
    }
    return (int)hash;
}

static int ht_get_hash(const char* s, const int num_buckets, const int attempt) {
    const int hash_a = ht_hash(s, HT_PRIME_1, num_buckets);
    const int hash_b = ht_hash(s, HT_PRIME_2, num_buckets);
    return (hash_a + attempt * (hash_b + 1)) % num_buckets;
}

/* ---------- CREATE & DELETE ---------- */
static ht_hash_table* ht_new_sized(const int base_size) {
    ht_hash_table* ht = xmalloc(sizeof(ht_hash_table));
    ht->base_size = base_size;
    ht->size = next_prime(ht->base_size);
    ht->count = 0;
    ht->items = xcalloc((size_t)ht->size, sizeof(ht_item*));
    return ht;
}

ht_hash_table* ht_new() {
    return ht_new_sized(HT_INITIAL_BASE_SIZE);
}

void ht_del_hash_table(ht_hash_table* ht) {
    for (int i = 0; i < ht->size; i++) {
        ht_item* item = ht->items[i];
        if (item != NULL && item != &HT_DELETED_ITEM) {
            ht_del_item(item);
        }
    }
    free(ht->items);
    free(ht);
}

/* ---------- INSERT, SEARCH, DELETE ---------- */
void ht_insert(ht_hash_table* ht, const char* key, const char* value) {
    ht_item* item = ht_new_item(key, value);
    int index;
    int i = 0;
    int first_deleted_index = -1;
    ht_item* cur_item;

    while (i < ht->size) {
        index = ht_get_hash(key, ht->size, i);
        cur_item = ht->items[index];

        if (cur_item == NULL) {
            // nếu trước đó gặp deleted, dùng slot deleted
            if (first_deleted_index != -1) index = first_deleted_index;
            ht->items[index] = item;
            ht->count++;
            return;
        }

        if (cur_item == &HT_DELETED_ITEM) {
            if (first_deleted_index == -1) {
                first_deleted_index = index; // lưu slot đầu tiên có deleted
            }
        } else if (strcmp(cur_item->key, key) == 0) {
            // key tồn tại → overwrite
            ht_del_item(cur_item);
            ht->items[index] = item;
            return;
        }

        i++;
    }

    // Nếu probing full và chưa chèn → dùng slot deleted đầu tiên
    if (first_deleted_index != -1) {
        ht->items[first_deleted_index] = item;
        ht->count++;
        return;
    }

    // bảng đầy → resize
    // ht_resize_up(ht); // nếu có resize logic
}

char* ht_search(ht_hash_table* ht, const char* key) {
    int i = 0;
    int index;
    ht_item* item;

    while (i < ht->size) {  // giới hạn probing
        index = ht_get_hash(key, ht->size, i);
        item = ht->items[index];

        if (item == NULL) {
            // chưa tìm thấy, dừng probing
            return NULL;
        }

        if (item != &HT_DELETED_ITEM && strcmp(item->key, key) == 0) {
            return item->value;  // tìm thấy
        }

        i++;  // probing tiếp
    }
    return NULL;  // không tìm thấy
}


void ht_delete(ht_hash_table* ht, const char* key) {
    int i = 0;
    int index;
    ht_item* item;

    while (i < ht->size) {
        index = ht_get_hash(key, ht->size, i);
        item = ht->items[index];

        if (item == NULL) {
            // key không tồn tại
            return;
        }

        if (item != &HT_DELETED_ITEM && strcmp(item->key, key) == 0) {
            ht_del_item(item);
            ht->items[index] = &HT_DELETED_ITEM;
            ht->count--;
            return;
        }

        i++;
    }
}

/* ---------- RESIZE ---------- */
static void ht_resize(ht_hash_table* ht, const int base_size) {
    if (base_size < HT_INITIAL_BASE_SIZE) return;

    ht_hash_table* new_ht = ht_new_sized(base_size);
    for (int i = 0; i < ht->size; i++) {
        ht_item* item = ht->items[i];
        if (item != NULL && item != &HT_DELETED_ITEM) {
            ht_insert(new_ht, item->key, item->value);
        }
    }

    // Swap
    ht->base_size = new_ht->base_size;
    ht->count = new_ht->count;

    int tmp_size = ht->size;
    ht->size = new_ht->size;
    new_ht->size = tmp_size;

    ht_item** tmp_items = ht->items;
    ht->items = new_ht->items;
    new_ht->items = tmp_items;

    ht_del_hash_table(new_ht);
}

static void ht_resize_up(ht_hash_table* ht) {
    ht_resize(ht, ht->base_size * 2);
}

static void ht_resize_down(ht_hash_table* ht) {
    ht_resize(ht, ht->base_size / 2);
}
