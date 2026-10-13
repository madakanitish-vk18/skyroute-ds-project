#ifndef HASHTABLE_H
#define HASHTABLE_H
/* Separate-chaining hash table: airport code -> airport index. */
#define HT_SIZE 31

typedef struct HNode {
    char key[4];
    int value;
    struct HNode *next;
} HNode;

typedef struct { HNode *buckets[HT_SIZE]; } HashTable;

void ht_init(HashTable *t);
int  ht_put(HashTable *t, const char *key, int value); /* 1 ok, 0 fail */
int  ht_get(const HashTable *t, const char *key);      /* value or -1  */
void ht_free(HashTable *t);
#endif
