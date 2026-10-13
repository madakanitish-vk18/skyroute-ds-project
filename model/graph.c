#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "graph.h"

void graph_init(Graph *g) {
    g->count = 0;
    ht_init(&g->ht);
    for (int i = 0; i < MAX_AIRPORTS; i++) {
        g->airports[i].head = NULL;
        g->airports[i].closed = 0;
    }
}

void graph_free(Graph *g) {
    for (int i = 0; i < g->count; i++) {
        Flight *f = g->airports[i].head;
        while (f) { Flight *nx = f->next; free(f); f = nx; }
        g->airports[i].head = NULL;
    }
    ht_free(&g->ht);
    g->count = 0;
}

void graph_normalize_code(char *code) {
    for (; *code; code++) *code = (char)toupper((unsigned char)*code);
}

int graph_valid(const Graph *g, int idx) { return idx >= 0 && idx < g->count; }

int graph_find(const Graph *g, const char *code) { return ht_get(&g->ht, code); }

int graph_add_airport(Graph *g, const char *code, const char *city) {
    if (g->count >= MAX_AIRPORTS || strlen(code) != 3 || !*city) return -1;
    for (int i = 0; i < 3; i++) if (!isalpha((unsigned char)code[i])) return -1;
    char norm[4];
    snprintf(norm, sizeof norm, "%s", code);
    graph_normalize_code(norm);
    if (graph_find(g, norm) >= 0) return -1;            /* duplicate */
    int idx = g->count;
    Airport *a = &g->airports[idx];
    snprintf(a->code, sizeof a->code, "%s", norm);
    snprintf(a->city, sizeof a->city, "%s", city);
    for (char *p = a->city; *p; p++) if (*p == ' ') *p = '_';
    a->closed = 0;
    a->head = NULL;
    if (!ht_put(&g->ht, a->code, idx)) return -1;
    g->count++;
    return idx;
}

Flight *graph_get_flight(const Graph *g, int s, int d) {
    if (!graph_valid(g, s)) return NULL;
    for (Flight *f = g->airports[s].head; f; f = f->next)
        if (f->dest == d) return f;
    return NULL;
}

int graph_add_flight(Graph *g, int s, int d, int cost, int dur, const char *airline) {
    if (!graph_valid(g, s) || !graph_valid(g, d) || s == d || cost <= 0 || dur <= 0) return 0;
    Flight *f = graph_get_flight(g, s, d);
    if (!f) {                                           /* new edge, insert at head */
        f = malloc(sizeof *f);
        if (!f) return 0;
        f->dest = d;
        f->next = g->airports[s].head;
        g->airports[s].head = f;
    }
    f->cost = cost;
    f->duration = dur;
    f->active = 1;
    snprintf(f->airline, sizeof f->airline, "%s", airline);
    return 1;
}

int graph_remove_flight(Graph *g, int s, int d) {
    if (!graph_valid(g, s)) return 0;
    Flight **pp = &g->airports[s].head;
    while (*pp) {
        if ((*pp)->dest == d) {
            Flight *dead = *pp;
            *pp = dead->next;
            free(dead);
            return 1;
        }
        pp = &(*pp)->next;
    }
    return 0;
}

int graph_set_flight_active(Graph *g, int s, int d, int active) {
    Flight *f = graph_get_flight(g, s, d);
    if (!f) return 0;
    f->active = active;
    return 1;
}

int graph_set_airport_closed(Graph *g, int idx, int closed) {
    if (!graph_valid(g, idx)) return 0;
    g->airports[idx].closed = closed;
    return 1;
}

/* Files: airports.txt -> "CODE City"   flights.txt -> "SRC DST cost minutes Airline" */
int graph_load(Graph *g, const char *airports_file, const char *flights_file) {
    FILE *fa = fopen(airports_file, "r");
    if (!fa) return 0;
    char code[8], city[64];
    while (fscanf(fa, "%7s %63s", code, city) == 2) graph_add_airport(g, code, city);
    fclose(fa);

    FILE *ff = fopen(flights_file, "r");
    if (!ff) return 0;
    char a[8], b[8], airline[32];
    int cost, dur;
    while (fscanf(ff, "%7s %7s %d %d %31s", a, b, &cost, &dur, airline) == 5)
        graph_add_flight(g, graph_find(g, a), graph_find(g, b), cost, dur, airline);
    fclose(ff);
    return 1;
}

/* Note: temporary disruptions (closures / cancellations) are not saved. */
int graph_save(const Graph *g, const char *airports_file, const char *flights_file) {
    FILE *fa = fopen(airports_file, "w");
    if (!fa) return 0;
    for (int i = 0; i < g->count; i++)
        fprintf(fa, "%s %s\n", g->airports[i].code, g->airports[i].city);
    fclose(fa);

    FILE *ff = fopen(flights_file, "w");
    if (!ff) return 0;
    for (int i = 0; i < g->count; i++)
        for (Flight *f = g->airports[i].head; f; f = f->next)
            fprintf(ff, "%s %s %d %d %s\n", g->airports[i].code,
                    g->airports[f->dest].code, f->cost, f->duration, f->airline);
    fclose(ff);
    return 1;
}
