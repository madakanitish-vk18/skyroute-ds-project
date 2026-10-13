#ifndef VIEW_H
#define VIEW_H
/* All console input/output lives here (the V in MVC). */
#include <stddef.h>
#include "graph.h"
#include "routing.h"

void view_banner(void);
void view_menu(void);
void view_message(const char *msg);
int  view_read_line(const char *prompt, char *buf, size_t n); /* 0 on EOF */
int  view_read_int(const char *prompt, int *out);             /* 1 if valid */
void view_show_airports(const Graph *g);
void view_show_flights_from(const Graph *g, int idx);
void view_show_route(const Graph *g, const Route *r, const char *title);
void view_show_alternatives(const Graph *g, const Route routes[], int n);
#endif
