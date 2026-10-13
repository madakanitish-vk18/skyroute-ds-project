#ifndef ROUTING_H
#define ROUTING_H
#include "graph.h"

typedef enum { METRIC_COST, METRIC_TIME } Metric;

typedef struct {
    int nodes[MAX_AIRPORTS];  /* airport indices from source to destination */
    int n;
    int cost;
    int time;
} Route;

/* Dijkstra (min-heap): cheapest or fastest route. Returns 1 if a route exists. */
int route_dijkstra(const Graph *g, int s, int d, Metric m, Route *out);
/* BFS (queue): route with the fewest flights. */
int route_fewest_layovers(const Graph *g, int s, int d, Route *out);
/* DFS (stack-like path, max 3 layovers): alternatives sorted by cost. Returns count. */
int route_alternatives(const Graph *g, int s, int d, Route out[], int max_out);
#endif
