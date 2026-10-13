#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "controller.h"

int main(int argc, char **argv) {
    const char *af = argc > 1 ? argv[1] : "data/airports.txt";
    const char *ff = argc > 2 ? argv[2] : "data/flights.txt";
    Graph *g = malloc(sizeof *g);
    if (!g) return 1;
    graph_init(g);
    if (!graph_load(g, af, ff)) {
        fprintf(stderr, "Could not load %s / %s (run from the project folder).\n", af, ff);
        free(g);
        return 1;
    }
    controller_run(g, af, ff);
    graph_free(g);
    free(g);
    return 0;
}
