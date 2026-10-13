#ifndef GRAPH_H
#define GRAPH_H
/* Weighted directed graph (adjacency list). Airports = vertices, flights = edges. */
#include "hashtable.h"

#define MAX_AIRPORTS 64

typedef struct Flight {
    int dest;
    int cost;          /* fare in rupees */
    int duration;      /* minutes */
    char airline[16];
    int active;        /* 0 = cancelled */
    struct Flight *next;
} Flight;

typedef struct {
    char code[4];
    char city[32];
    int closed;        /* 1 = airport closed */
    Flight *head;
} Airport;

typedef struct {
    Airport airports[MAX_AIRPORTS];
    int count;
    HashTable ht;      /* code -> index */
} Graph;

void  graph_init(Graph *g);
void  graph_free(Graph *g);
void  graph_normalize_code(char *code);                 /* upper-case in place */
int   graph_valid(const Graph *g, int idx);
int   graph_find(const Graph *g, const char *code);     /* index or -1 */
int   graph_add_airport(Graph *g, const char *code, const char *city); /* index or -1 */
int   graph_add_flight(Graph *g, int s, int d, int cost, int dur, const char *airline); /* 1 ok */
int   graph_remove_flight(Graph *g, int s, int d);
Flight *graph_get_flight(const Graph *g, int s, int d);
int   graph_set_flight_active(Graph *g, int s, int d, int active);
int   graph_set_airport_closed(Graph *g, int idx, int closed);
int   graph_load(Graph *g, const char *airports_file, const char *flights_file);
int   graph_save(const Graph *g, const char *airports_file, const char *flights_file);
#endif
