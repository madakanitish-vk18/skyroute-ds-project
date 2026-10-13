#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "view.h"

static void print_city(const char *c) {
    for (; *c; c++) putchar(*c == '_' ? ' ' : *c);
}

void view_banner(void) {
    printf("\n==========================================\n");
    printf("   SkyRoute - Flight Route Optimiser (C)\n");
    printf("==========================================\n");
}

void view_menu(void) {
    printf("\n--- MENU ---\n");
    printf(" 1. List all airports\n");
    printf(" 2. Search airport / direct flights\n");
    printf(" 3. Cheapest route\n");
    printf(" 4. Fastest route\n");
    printf(" 5. Fewest layovers\n");
    printf(" 6. Top 3 alternative routes (by fare)\n");
    printf(" 7. Add airport\n");
    printf(" 8. Add / update flight\n");
    printf(" 9. Remove flight\n");
    printf("10. Close / reopen airport\n");
    printf("11. Cancel / restore flight\n");
    printf("12. Save data\n");
    printf(" 0. Exit\n");
}

void view_message(const char *msg) { printf("%s\n", msg); }

int view_read_line(const char *prompt, char *buf, size_t n) {
    printf("%s", prompt);
    fflush(stdout);
    if (!fgets(buf, (int)n, stdin)) return 0;
    buf[strcspn(buf, "\r\n")] = '\0';
    return 1;
}

int view_read_int(const char *prompt, int *out) {
    char buf[32], *end;
    if (!view_read_line(prompt, buf, sizeof buf)) return 0;
    long v = strtol(buf, &end, 10);
    if (end == buf || *end != '\0') return 0;
    *out = (int)v;
    return 1;
}

void view_show_airports(const Graph *g) {
    printf("\n%-5s %-16s %s\n", "Code", "City", "Status");
    for (int i = 0; i < g->count; i++) {
        printf("%-5s ", g->airports[i].code);
        char tmp[32];
        snprintf(tmp, sizeof tmp, "%s", g->airports[i].city);
        for (char *p = tmp; *p; p++) if (*p == '_') *p = ' ';
        printf("%-16s %s\n", tmp, g->airports[i].closed ? "CLOSED" : "open");
    }
}

void view_show_flights_from(const Graph *g, int idx) {
    const Airport *a = &g->airports[idx];
    printf("\n%s - ", a->code);
    print_city(a->city);
    printf(" (%s)\nDirect flights:\n", a->closed ? "CLOSED" : "open");
    int any = 0;
    for (Flight *f = a->head; f; f = f->next) {
        any = 1;
        printf("  -> %s  %-10s Rs %5d  %3dh %02dm %s\n", g->airports[f->dest].code,
               f->airline, f->cost, f->duration / 60, f->duration % 60,
               f->active ? "" : "[CANCELLED]");
    }
    if (!any) printf("  (none)\n");
}

void view_show_route(const Graph *g, const Route *r, const char *title) {
    printf("\n=== %s ===\n", title);
    for (int i = 0; i < r->n; i++) {
        const Airport *a = &g->airports[r->nodes[i]];
        printf("%s (", a->code);
        print_city(a->city);
        printf(")%s", i + 1 < r->n ? " -> " : "\n");
    }
    for (int i = 0; i + 1 < r->n; i++) {
        Flight *f = graph_get_flight(g, r->nodes[i], r->nodes[i + 1]);
        printf("  %s -> %s  %-10s Rs %5d  %3dh %02dm\n", g->airports[r->nodes[i]].code,
               g->airports[r->nodes[i + 1]].code, f->airline, f->cost,
               f->duration / 60, f->duration % 60);
    }
    printf("Total: Rs %d | %dh %02dm | Layovers: %d\n", r->cost, r->time / 60,
           r->time % 60, r->n - 2);
}

void view_show_alternatives(const Graph *g, const Route routes[], int n) {
    printf("\n=== Top %d routes by fare ===\n", n);
    for (int i = 0; i < n; i++) {
        printf("%d) ", i + 1);
        for (int j = 0; j < routes[i].n; j++)
            printf("%s%s", g->airports[routes[i].nodes[j]].code, j + 1 < routes[i].n ? " -> " : "");
        printf("  | Rs %d | %dh %02dm | Layovers: %d\n", routes[i].cost,
               routes[i].time / 60, routes[i].time % 60, routes[i].n - 2);
    }
}
