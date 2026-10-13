#include <stdio.h>
#include <string.h>
#include "controller.h"
#include "view.h"
#include "routing.h"

static int dirty = 0;   /* unsaved changes? */

/* Returns airport index, or -1 if unknown / input ended. */
static int ask_airport(const Graph *g, const char *prompt) {
    char buf[32];
    if (!view_read_line(prompt, buf, sizeof buf)) return -1;
    graph_normalize_code(buf);
    int idx = graph_find(g, buf);
    if (idx < 0) view_message("Unknown airport code.");
    return idx;
}

typedef enum { Q_CHEAPEST, Q_FASTEST, Q_FEWEST } Query;

static void do_route_query(const Graph *g, Query q) {
    int s = ask_airport(g, "From (code): ");
    if (s < 0) return;
    int d = ask_airport(g, "To (code): ");
    if (d < 0) return;
    if (s == d) { view_message("Source and destination are the same."); return; }
    Route r;
    int ok = 0;
    const char *title = "";
    switch (q) {
        case Q_CHEAPEST: ok = route_dijkstra(g, s, d, METRIC_COST, &r); title = "Cheapest route"; break;
        case Q_FASTEST:  ok = route_dijkstra(g, s, d, METRIC_TIME, &r); title = "Fastest route"; break;
        case Q_FEWEST:   ok = route_fewest_layovers(g, s, d, &r);       title = "Fewest layovers"; break;
    }
    if (ok) view_show_route(g, &r, title);
    else view_message("No route available (check closures / cancellations).");
}

static void do_alternatives(const Graph *g) {
    int s = ask_airport(g, "From (code): ");
    if (s < 0) return;
    int d = ask_airport(g, "To (code): ");
    if (d < 0) return;
    if (s == d) { view_message("Source and destination are the same."); return; }
    Route routes[3];
    int n = route_alternatives(g, s, d, routes, 3);
    if (n == 0) view_message("No route available (max 3 layovers considered).");
    else view_show_alternatives(g, routes, n);
}

static void do_search(const Graph *g) {
    int idx = ask_airport(g, "Airport code: ");
    if (idx >= 0) view_show_flights_from(g, idx);
}

static void do_add_airport(Graph *g) {
    char code[32], city[64];
    if (!view_read_line("New airport code (3 letters): ", code, sizeof code)) return;
    if (!view_read_line("City name: ", city, sizeof city)) return;
    if (graph_add_airport(g, code, city) >= 0) { view_message("Airport added."); dirty = 1; }
    else view_message("Could not add (invalid, duplicate, or list full).");
}

static void do_add_flight(Graph *g) {
    int s = ask_airport(g, "From (code): ");
    if (s < 0) return;
    int d = ask_airport(g, "To (code): ");
    if (d < 0) return;
    int cost, dur;
    char airline[32], yn[8];
    if (!view_read_int("Fare (Rs): ", &cost) || !view_read_int("Duration (minutes): ", &dur)) {
        view_message("Invalid number."); return;
    }
    if (!view_read_line("Airline (one word): ", airline, sizeof airline)) return;
    if (!graph_add_flight(g, s, d, cost, dur, airline)) { view_message("Could not add flight."); return; }
    if (view_read_line("Add the return flight too? (y/n): ", yn, sizeof yn) && (yn[0] == 'y' || yn[0] == 'Y'))
        graph_add_flight(g, d, s, cost, dur, airline);
    view_message("Flight saved in memory.");
    dirty = 1;
}

static void do_remove_flight(Graph *g) {
    int s = ask_airport(g, "From (code): ");
    if (s < 0) return;
    int d = ask_airport(g, "To (code): ");
    if (d < 0) return;
    if (graph_remove_flight(g, s, d)) { view_message("Flight removed."); dirty = 1; }
    else view_message("No such flight.");
}

static void do_close_airport(Graph *g) {
    int idx = ask_airport(g, "Airport code: ");
    if (idx < 0) return;
    int v;
    if (!view_read_int("1 = close, 0 = reopen: ", &v) || (v != 0 && v != 1)) { view_message("Invalid choice."); return; }
    graph_set_airport_closed(g, idx, v);
    view_message(v ? "Airport closed. Routes will avoid it." : "Airport reopened.");
}

static void do_cancel_flight(Graph *g) {
    int s = ask_airport(g, "From (code): ");
    if (s < 0) return;
    int d = ask_airport(g, "To (code): ");
    if (d < 0) return;
    int v;
    if (!view_read_int("1 = cancel, 0 = restore: ", &v) || (v != 0 && v != 1)) { view_message("Invalid choice."); return; }
    if (graph_set_flight_active(g, s, d, !v)) view_message(v ? "Flight cancelled." : "Flight restored.");
    else view_message("No such flight.");
}

static void do_save(const Graph *g, const char *af, const char *ff) {
    if (graph_save(g, af, ff)) { view_message("Data saved."); dirty = 0; }
    else view_message("Could not write data files.");
}

void controller_run(Graph *g, const char *af, const char *ff) {
    view_banner();
    for (;;) {
        view_menu();
        int choice;
        if (!view_read_int("Choice: ", &choice)) {
            if (feof(stdin)) break;
            view_message("Please enter a number from the menu.");
            continue;
        }
        switch (choice) {
            case 1:  view_show_airports(g); break;
            case 2:  do_search(g); break;
            case 3:  do_route_query(g, Q_CHEAPEST); break;
            case 4:  do_route_query(g, Q_FASTEST); break;
            case 5:  do_route_query(g, Q_FEWEST); break;
            case 6:  do_alternatives(g); break;
            case 7:  do_add_airport(g); break;
            case 8:  do_add_flight(g); break;
            case 9:  do_remove_flight(g); break;
            case 10: do_close_airport(g); break;
            case 11: do_cancel_flight(g); break;
            case 12: do_save(g, af, ff); break;
            case 0:
                if (dirty) {
                    char yn[8];
                    if (view_read_line("Unsaved changes. Save now? (y/n): ", yn, sizeof yn) &&
                        (yn[0] == 'y' || yn[0] == 'Y'))
                        do_save(g, af, ff);
                }
                view_message("Goodbye!");
                return;
            default: view_message("Invalid choice.");
        }
    }
}
