# SkyRoute - Test Cases

Each test starts from the original sample data (`data/airports.txt`, `data/flights.txt`). Run `./skyroute` (Windows: `.\skyroute.exe`) and type the steps one per line.
The results below were produced by running the program (also checked with AddressSanitizer: no memory errors).

| ID | What is tested | Expected result |
|---|---|---|
| T1 | Cheapest route (Dijkstra, fare) | Cheapest Hyderabad to Jaipur |
| T2 | Fastest route (Dijkstra, duration) | Fastest Hyderabad to Jaipur (differs from T1) |
| T3 | Fewest layovers (BFS) | Fewest layovers Hyderabad to Kochi |
| T4 | Top 3 alternative routes | Three cheapest routes Hyderabad to Kochi |
| T5 | Airport closure and automatic rerouting | Route before and after closing Bengaluru (BLR), then reopen |
| T6 | Flight cancellation and rerouting | Cancel HYD-BOM flight; the cheapest route changes |
| T7 | Search airport / direct flights | Direct flights from Visakhapatnam |
| T8 | Destination airport closed | No route when the destination is closed |
| T9 | Unknown airport code | Error message for a code that does not exist |
| T10 | Same source and destination | Error message |
| T11 | Add airport and flights, then route to it | New airport reachable through the new flight |
| T12 | Remove a flight | Second removal reports that the flight does not exist |
| T13 | Invalid menu input | Non-numeric and out-of-range choices are rejected |
| T14 | Invalid number while adding a flight | Rejected without crashing |
| T15 | Data persists after Save (two runs) | Airport and flights added in run 1 are still there in run 2 |

## T1: Cheapest route (Dijkstra, fare)

**Steps typed:** `3 , HYD , JAI`

**Output:**

```
=== Cheapest route ===
HYD (Hyderabad) -> BOM (Mumbai) -> JAI (Jaipur)
  HYD -> BOM  Air_India  Rs  3600    1h 30m
  BOM -> JAI  IndiGo     Rs  4400    1h 50m
Total: Rs 8000 | 3h 20m | Layovers: 1
Goodbye!
```

## T2: Fastest route (Dijkstra, duration)

**Steps typed:** `4 , HYD , JAI`

**Output:**

```
=== Fastest route ===
HYD (Hyderabad) -> DEL (Delhi) -> JAI (Jaipur)
  HYD -> DEL  IndiGo     Rs  5200    2h 20m
  DEL -> JAI  IndiGo     Rs  3200    0h 55m
Total: Rs 8400 | 3h 15m | Layovers: 1
Goodbye!
```

## T3: Fewest layovers (BFS)

**Steps typed:** `5 , HYD , COK`

**Output:**

```
=== Fewest layovers ===
HYD (Hyderabad) -> MAA (Chennai) -> COK (Kochi)
  HYD -> MAA  IndiGo     Rs  2900    1h 15m
  MAA -> COK  IndiGo     Rs  3300    1h 10m
Total: Rs 6200 | 2h 25m | Layovers: 1
Goodbye!
```

## T4: Top 3 alternative routes

**Steps typed:** `6 , HYD , COK`

**Output:**

```
=== Top 3 routes by fare ===
1) HYD -> BLR -> COK  | Rs 5800 | 2h 30m | Layovers: 1
2) HYD -> MAA -> COK  | Rs 6200 | 2h 25m | Layovers: 1
3) HYD -> BOM -> COK  | Rs 8300 | 3h 30m | Layovers: 1
Goodbye!
```

## T5: Airport closure and automatic rerouting

**Steps typed:** `3 , HYD , COK , 10 , BLR , 1 , 3 , HYD , COK , 10 , BLR , 0`

**Output:**

```
=== Cheapest route ===
HYD (Hyderabad) -> BLR (Bengaluru) -> COK (Kochi)
  HYD -> BLR  IndiGo     Rs  2800    1h 20m
  BLR -> COK  IndiGo     Rs  3000    1h 10m
Total: Rs 5800 | 2h 30m | Layovers: 1
Airport closed. Routes will avoid it.
=== Cheapest route ===
HYD (Hyderabad) -> MAA (Chennai) -> COK (Kochi)
  HYD -> MAA  IndiGo     Rs  2900    1h 15m
  MAA -> COK  IndiGo     Rs  3300    1h 10m
Total: Rs 6200 | 2h 25m | Layovers: 1
Airport reopened.
Goodbye!
```

## T6: Flight cancellation and rerouting

**Steps typed:** `3 , HYD , JAI , 11 , HYD , BOM , 1 , 3 , HYD , JAI`

**Output:**

```
=== Cheapest route ===
HYD (Hyderabad) -> BOM (Mumbai) -> JAI (Jaipur)
  HYD -> BOM  Air_India  Rs  3600    1h 30m
  BOM -> JAI  IndiGo     Rs  4400    1h 50m
Total: Rs 8000 | 3h 20m | Layovers: 1
Flight cancelled.
=== Cheapest route ===
HYD (Hyderabad) -> DEL (Delhi) -> JAI (Jaipur)
  HYD -> DEL  IndiGo     Rs  5200    2h 20m
  DEL -> JAI  IndiGo     Rs  3200    0h 55m
Total: Rs 8400 | 3h 15m | Layovers: 1
Goodbye!
```

## T7: Search airport / direct flights

**Steps typed:** `2 , VTZ`

**Output:**

```
VTZ - Visakhapatnam (open)
Direct flights:
  -> CCU  IndiGo     Rs  4700    1h 45m
  -> MAA  IndiGo     Rs  3500    1h 20m
  -> HYD  IndiGo     Rs  3000    1h 10m
Goodbye!
```

## T8: Destination airport closed

**Steps typed:** `10 , COK , 1 , 3 , HYD , COK`

**Output:**

```
Airport closed. Routes will avoid it.
No route available (check closures / cancellations).
Goodbye!
```

## T9: Unknown airport code

**Steps typed:** `3 , XYZ`

**Output:**

```
Unknown airport code.
Goodbye!
```

## T10: Same source and destination

**Steps typed:** `3 , HYD , HYD`

**Output:**

```
Source and destination are the same.
Goodbye!
```

## T11: Add airport and flights, then route to it

**Steps typed:** `7 , GAU , Guwahati , 8 , CCU , GAU , 5500 , 110 , IndiGo , y , 3 , HYD , GAU`

**Output:**

```
Airport added.
Flight saved in memory.
=== Cheapest route ===
HYD (Hyderabad) -> CCU (Kolkata) -> GAU (Guwahati)
  HYD -> CCU  IndiGo     Rs  5100    2h 20m
  CCU -> GAU  IndiGo     Rs  5500    1h 50m
Total: Rs 10600 | 4h 10m | Layovers: 1
Goodbye!
```

## T12: Remove a flight

**Steps typed:** `9 , HYD , BOM , 9 , HYD , BOM`

**Output:**

```
Flight removed.
No such flight.
Goodbye!
```

## T13: Invalid menu input

**Steps typed:** `abc , 99`

**Output:**

```
Please enter a number from the menu.
Invalid choice.
Goodbye!
```

## T14: Invalid number while adding a flight

**Steps typed:** `8 , HYD , DEL , abc`

**Output:**

```
Invalid number.
Goodbye!
```

## T15: Persistence

**Run 1 typed:** `7 , GAU , Guwahati , 8 , CCU , GAU , 5500 , 110 , Air India , y , 12 , 0`  (adds Guwahati, a flight with a two-word airline name, then Save)

**Run 2 typed:** `3 , HYD , GAU , 0`

**Output of run 2:**

```
=== Cheapest route ===
HYD (Hyderabad) -> CCU (Kolkata) -> GAU (Guwahati)
  HYD -> CCU  IndiGo     Rs  5100    2h 20m
  CCU -> GAU  Air_India  Rs  5500    1h 50m
Total: Rs 10600 | 4h 10m | Layovers: 1
Goodbye!
```
