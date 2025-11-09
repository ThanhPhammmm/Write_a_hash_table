#include "hash_table.h"

int main() {
    printf("=== Hash Table Test Start ===\n");

    ht_hash_table* ht = ht_new();
    if (!ht) {
        fprintf(stderr, "Failed to create hash table!\n");
        return 1;
    }
    ht_insert(ht, "apple", "5");
    ht_insert(ht, "banana", "7");
    ht_insert(ht, "orange", "10");
    ht_insert(ht, "grape", "3");
    ht_insert(ht, "melon", "12");
    ht_insert(ht, "kiwi", "8");

    printf("apple → %s\n", ht_search(ht, "apple"));    // 5

    ht_insert(ht, "apple", "15"); // update giá trị

    printf("apple → %s\n", ht_search(ht, "apple"));    // 15
    printf("banana → %s\n", ht_search(ht, "banana"));  // 7
    printf("orange → %s\n", ht_search(ht, "orange"));  // 10
    printf("grape → %s\n", ht_search(ht, "grape"));    // 3
    printf("melon → %s\n", ht_search(ht, "melon"));    // 12
    printf("kiwi → %s\n", ht_search(ht, "kiwi"));      // 8
    printf("nonexistent → %s\n", ht_search(ht, "pear")); // NULL

    printf("== Current counts: %d ==\n", ht->count);
    ht_delete(ht, "apple");
    printf("apple after delete → %s\n", ht_search(ht, "apple")); // NULL
    printf("== Current counts: %d ==\n", ht->count);

    //resize up
    for (int i = 0; i < 100; i++) {
        char key[32], value[16]; 
        snprintf(key, sizeof(key), "key%d", i); 
        snprintf(value, sizeof(value), "%d", i*10); 
        ht_insert(ht, key, value);
    }
    printf("== Current counts: %d ==\n", ht->count);

    printf("key50 → %s\n", ht_search(ht, "key50")); // 500
    printf("key99 → %s\n", ht_search(ht, "key99")); // 990

    //resize down
    for (int i = 0; i < 100; i += 2) {
        char key[32];
        snprintf(key, sizeof(key), "key%d", i); 
        ht_delete(ht, key);
    }

    printf("key50 after delete → %s\n", ht_search(ht, "key50")); // NULL
    printf("key51 → %s\n", ht_search(ht, "key51")); // 510

    ht_del_hash_table(ht);

    printf("=== Hash Table Test Finished ===\n");
    return 0;
}
