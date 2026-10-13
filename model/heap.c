#include <stdlib.h>
#include "heap.h"

void heap_init(MinHeap *h, int cap) {
    h->data = malloc(sizeof(HeapItem) * cap);
    h->size = 0;
    h->cap = h->data ? cap : 0;
}

static void swap_items(HeapItem *a, HeapItem *b) { HeapItem t = *a; *a = *b; *b = t; }

int heap_push(MinHeap *h, int node, int key) {
    if (h->size == h->cap) {
        int ncap = h->cap ? h->cap * 2 : 16;
        HeapItem *nd = realloc(h->data, sizeof(HeapItem) * ncap);
        if (!nd) return 0;
        h->data = nd; h->cap = ncap;
    }
    int i = h->size++;
    h->data[i].node = node; h->data[i].key = key;
    while (i > 0) {                       /* sift up */
        int p = (i - 1) / 2;
        if (h->data[p].key <= h->data[i].key) break;
        swap_items(&h->data[p], &h->data[i]);
        i = p;
    }
    return 1;
}

int heap_pop(MinHeap *h, HeapItem *out) {
    if (h->size == 0) return 0;
    *out = h->data[0];
    h->data[0] = h->data[--h->size];
    int i = 0;
    for (;;) {                            /* sift down */
        int l = 2 * i + 1, r = l + 1, m = i;
        if (l < h->size && h->data[l].key < h->data[m].key) m = l;
        if (r < h->size && h->data[r].key < h->data[m].key) m = r;
        if (m == i) break;
        swap_items(&h->data[m], &h->data[i]);
        i = m;
    }
    return 1;
}

void heap_free(MinHeap *h) { free(h->data); h->data = NULL; h->size = h->cap = 0; }
