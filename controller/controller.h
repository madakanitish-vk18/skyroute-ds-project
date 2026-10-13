#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "graph.h"
/* Runs the menu loop: reads user choices, calls the Model, tells the View what to show. */
void controller_run(Graph *g, const char *airports_file, const char *flights_file);
#endif
