#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "hashtable.h"

static unsigned hash_key(const char *k) {
    unsigned h = 5381;
    while (*k) h = h * 33 + (unsigned char)*k++;
    return h % HT_SIZE;
}

void ht_init(HashTable *t) {
    for (int i = 0; i < HT_SIZE; i++) t->buckets[i] = NULL;
}

int ht_put(HashTable *t, const char *key, int value) {
    unsigned i = hash_key(key);
    for (HNode *n = t->buckets[i]; n; n = n->next)
        if (strcmp(n->key, key) == 0) { n->value = value; return 1; }
    HNode *n = malloc(sizeof *n);
    if (!n) return 0;
    snprintf(n->key, sizeof n->key, "%s", key);
    n->value = value;
    n->next = t->buckets[i];
    t->buckets[i] = n;
    return 1;
}

int ht_get(const HashTable *t, const char *key) {
    for (HNode *n = t->buckets[hash_key(key)]; n; n = n->next)
        if (strcmp(n->key, key) == 0) return n->value;
    return -1;
}

void ht_free(HashTable *t) {
    for (int i = 0; i < HT_SIZE; i++) {
        HNode *n = t->buckets[i];
        while (n) { HNode *nx = n->next; free(n); n = nx; }
        t->buckets[i] = NULL;
    }
}
