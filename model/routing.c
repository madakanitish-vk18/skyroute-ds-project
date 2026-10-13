#include <stdlib.h>
#include <string.h>
#include "routing.h"
#include "heap.h"

#define INF 1000000000
#define MAX_HOPS 4          /* flights per route => at most 3 layovers */
#define ALT_LIMIT 512

static int usable(const Graph *g, int s, int d) {
    return graph_valid(g, s) && graph_valid(g, d) &&
           !g->airports[s].closed && !g->airports[d].closed;
}

static int edge_ok(const Graph *g, const Flight *f) {
    return f->active && !g->airports[f->dest].closed;
}

/* Fill total cost/time of a route from its node list. */
static void fill_totals(const Graph *g, Route *r) {
    r->cost = r->time = 0;
    for (int i = 0; i + 1 < r->n; i++) {
        Flight *f = graph_get_flight(g, r->nodes[i], r->nodes[i + 1]);
        r->cost += f->cost;
        r->time += f->duration;
    }
}

/* Rebuild path from prev[] using a stack (reverses dest -> source). */
static void build_route(const Graph *g, const int prev[], int d, Route *out) {
    int stack[MAX_AIRPORTS], top = 0;
    for (int v = d; v != -1; v = prev[v]) stack[top++] = v;
    out->n = 0;
    while (top > 0) out->nodes[out->n++] = stack[--top];
    fill_totals(g, out);
}

int route_dijkstra(const Graph *g, int s, int d, Metric m, Route *out) {
    if (!usable(g, s, d)) return 0;
    int dist[MAX_AIRPORTS], prev[MAX_AIRPORTS];
    for (int i = 0; i < g->count; i++) { dist[i] = INF; prev[i] = -1; }
    MinHeap h;
    heap_init(&h, 32);
    dist[s] = 0;
    heap_push(&h, s, 0);
    HeapItem it;
    while (heap_pop(&h, &it)) {
        int u = it.node;
        if (it.key > dist[u]) continue;            /* stale entry */
        if (u == d) break;
        for (Flight *f = g->airports[u].head; f; f = f->next) {
            if (!edge_ok(g, f)) continue;
            int w = (m == METRIC_COST) ? f->cost : f->duration;
            if (dist[u] + w < dist[f->dest]) {
                dist[f->dest] = dist[u] + w;
                prev[f->dest] = u;
                heap_push(&h, f->dest, dist[f->dest]);
            }
        }
    }
    heap_free(&h);
    if (dist[d] == INF) return 0;
    build_route(g, prev, d, out);
    return 1;
}

int route_fewest_layovers(const Graph *g, int s, int d, Route *out) {
    if (!usable(g, s, d)) return 0;
    int queue[MAX_AIRPORTS], head = 0, tail = 0;
    int prev[MAX_AIRPORTS], seen[MAX_AIRPORTS];
    for (int i = 0; i < g->count; i++) { prev[i] = -1; seen[i] = 0; }
    queue[tail++] = s;
    seen[s] = 1;
    while (head < tail) {
        int u = queue[head++];
        if (u == d) break;
        for (Flight *f = g->airports[u].head; f; f = f->next) {
            if (!edge_ok(g, f) || seen[f->dest]) continue;
            seen[f->dest] = 1;
            prev[f->dest] = u;
            queue[tail++] = f->dest;
        }
    }
    if (!seen[d]) return 0;
    build_route(g, prev, d, out);
    return 1;
}

typedef struct {
    const Graph *g;
    int dst;
    int visited[MAX_AIRPORTS];
    int path[MAX_HOPS + 1];
    Route *found;
    int count;
} AltCtx;

static void dfs(AltCtx *c, int u, int depth) {
    c->path[depth] = u;
    c->visited[u] = 1;
    if (u == c->dst) {
        if (c->count < ALT_LIMIT) {
            Route *r = &c->found[c->count++];
            r->n = depth + 1;
            memcpy(r->nodes, c->path, sizeof(int) * r->n);
            fill_totals(c->g, r);
        }
    } else if (depth + 1 < MAX_HOPS + 1) {
        for (Flight *f = c->g->airports[u].head; f; f = f->next)
            if (edge_ok(c->g, f) && !c->visited[f->dest]) dfs(c, f->dest, depth + 1);
    }
    c->visited[u] = 0;
}

static int cmp_routes(const void *a, const void *b) {
    const Route *x = a, *y = b;
    if (x->cost != y->cost) return x->cost - y->cost;
    return x->time - y->time;
}

int route_alternatives(const Graph *g, int s, int d, Route out[], int max_out) {
    if (!usable(g, s, d)) return 0;
    AltCtx *c = calloc(1, sizeof *c);
    if (!c) return 0;
    c->found = malloc(sizeof(Route) * ALT_LIMIT);
    if (!c->found) { free(c); return 0; }
    c->g = g;
    c->dst = d;
    dfs(c, s, 0);
    qsort(c->found, c->count, sizeof(Route), cmp_routes);
    int n = c->count < max_out ? c->count : max_out;
    for (int i = 0; i < n; i++) out[i] = c->found[i];
    free(c->found);
    free(c);
    return n;
}
