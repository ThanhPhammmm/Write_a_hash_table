#include "hash_table.h"

int main() {
    ht_hash_table* ht = ht_new();
    ht_insert(ht, "apple", "5");
    ht_insert(ht, "banana", "7");

    printf("apple → %s\n", ht_search(ht, "apple"));
    printf("banana → %s\n", ht_search(ht, "banana"));

    ht_delete(ht, "apple");
    printf("apple after delete → %s\n", ht_search(ht, "apple"));

    ht_del_hash_table(ht);
    return 0;
}
