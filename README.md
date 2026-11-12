# SkyRoute - Flight Route Optimisation System (C)

A console application written in C that finds the cheapest, fastest and simplest flight routes between airports.
Airports are stored as a **weighted directed graph**; routes are found with **Dijkstra's algorithm (min-heap)**,
**BFS** and a bounded **DFS**; airport codes are looked up with a **hash table**.
Built with the **MVC (Model-View-Controller)** architecture as part of the Data Structures course-based project
(B.E II Year, III Semester, Vasavi College of Engineering, Dept. of IT).

## Features (use cases)
1. Add airports and flights, update fares, remove flights
2. Cheapest route (by fare)
3. Fastest route (by duration)
4. Route with the fewest layovers
5. Top 3 alternative routes (by fare, up to 3 layovers)
6. Automatic rerouting when an airport is closed
7. Automatic rerouting when a flight is cancelled / restored
8. Search an airport and list its direct flights
9. Load from and save to text files

## Data structures and algorithms
| Data structure | Used for | Cost |
|---|---|---|
| Weighted directed graph (adjacency list) | Airports = vertices, flights = edges | O(V + E) space |
| Min-heap priority queue | Picking the next airport in Dijkstra | O(log n) push/pop |
| Hash table (separate chaining) | Airport code -> graph index | O(1) average lookup |
| Queue (array) | BFS: fewest layovers | O(1) |
| Stack (array) | Rebuilding a route from destination to source | O(1) |

| Algorithm | Purpose | Time |
|---|---|---|
| Dijkstra with min-heap | Cheapest / fastest route (weights are never negative) | O((V + E) log V) |
| BFS | Fewest layovers | O(V + E) |
| Bounded DFS + sort | Top 3 alternatives (max 3 layovers) | exponential in the layover limit, small for 3 |

## Project structure (MVC)
```
main.c                  program entry: loads data, starts the controller
model/                  data structures and algorithms (no console input/output)
    hashtable.c/.h      hash table
    heap.c/.h           min-heap
    graph.c/.h          graph, load/save of data files
    routing.c/.h        Dijkstra, BFS, alternatives
view/view.c/.h          all printf/scanf: menus, prompts, route tables
controller/             reads the user's choice, calls the model, asks the view to show the result
data/airports.txt       sample airports
data/flights.txt        sample flights
TEST_CASES.md           15 documented test cases with output
```

## Build and run
Run all commands from the project folder (the program reads `data/` from the current folder).

**Windows (MinGW gcc, e.g. in the VS Code terminal)**
```
gcc -Wall -Wextra -std=c99 -Imodel -Iview -Icontroller -o skyroute main.c model/hashtable.c model/heap.c model/graph.c model/routing.c view/view.c controller/controller.c
.\skyroute.exe
```
**Linux / macOS / GitHub Codespaces**
```
make
./skyroute
```

## Data file format
- `airports.txt`: one airport per line: `CODE City` (use `_` instead of spaces in a city name)
- `flights.txt`: one flight per line: `FROM TO fare minutes Airline` (use `_` in airline names)
- Flights are one-way, so a two-way route has two lines. Malformed lines are skipped.
- The fares and durations in the sample data are made up for demonstration.

## Limitations
- Sample data only; no live fares or schedules.
- "Fastest" adds flight durations; layover waiting time and time of day are not modelled.
- Fewest-layovers returns one route with the fewest flights, not necessarily the cheapest of them.
- Maximum 64 airports; alternatives consider at most 3 layovers.
- Closures and cancellations are temporary and are not saved to the files.
- Console interface only.

## Future scope
Live schedules and layover times, cheapest route under a budget, graphical map interface, user accounts and booking, larger real-world datasets.

## Authors
Madaka Nitish (1602-25-737-029) and [partner name, roll number]
