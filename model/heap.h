#ifndef HEAP_H
#define HEAP_H
/* Binary min-heap used as the priority queue for Dijkstra. */
typedef struct { int node; int key; } HeapItem;
typedef struct { HeapItem *data; int size; int cap; } MinHeap;

void heap_init(MinHeap *h, int cap);
int  heap_push(MinHeap *h, int node, int key); /* 1 ok, 0 fail */
int  heap_pop(MinHeap *h, HeapItem *out);      /* 1 if item returned */
void heap_free(MinHeap *h);
#endif
