# Air Traffic Control Simulator: Sorted Vector vs. Heaps

CSCI 3110 (Algorithms and Data Structures) semester team project, MTSU, Fall 2026.
Team: William Starks, Kyle Penton, Pau Mang, Heidtmann Oppong.

An airport tower keeps a list of incoming flights and must always land the one due soonest. Flights join the list, run late or early, and get cancelled or diverted. We store that list three ways, a **sorted vector**, a **binary heap with lazy deletion** and an **indexed heap**, and measure which keeps up best as traffic grows. The data is 109,781 real flights into Atlanta (ATL) from May to August 2026.

## Status (November 12)

| Part | State |
|---|---|
| Real data, loader, simulator, benchmark, tests | Done |
| Structure A: sorted vector | Done |
| Structure B: heap with lazy deletion | Done. Passes every test and lands all 107,500 flights in the real order |
| Structure C: indexed heap | Done. Passes every test and lands all 107,500 flights in the real order. Write-up: `docs/research-indexed-heap.md` |
| Final benchmark and charts | Done: `results/results.csv`, `results/charts/` |
| Final report and slides | Done. The report (`ATC_Final_Report.docx`) is submitted separately from this repo; the slides are presented in class |

## Who's building what

| Who | Builds | Files | Done when | Target date |
|---|---|---|---|---|
| **William** | Done: the data, loader, simulator, benchmark, tests and Structure A (sorted vector). From here: reviews and merges pull requests, and combines everyone's slides into one deck | `data/`, `scripts/prepare_data.py`, `src/` except the two heaps, `tests/` | Code done. Deck combined | Nov 29 |
| **Kyle** | Structure B: a binary heap with lazy deletion | `src/LazyHeapQueue.h` and `.cpp` | `make test` passes for lazy-heap and `./atc check` says OK | Nov 12 |
| **Pau** | Structure C: an indexed heap, our independent research. Then a one-page write-up of how it works and whether it beats lazy deletion on our data | `src/IndexedHeapQueue.h` and `.cpp`, and a new `docs/research-indexed-heap.md` | `make test` passes for indexed-heap and `./atc check` says OK | Heap Nov 12, write-up Nov 22 |
| **Heidtmann** | The experiments and the final report. Writes a script that turns `results/results.csv` into a chart and a results table, then runs the final benchmark once both heaps work. Then puts the final report together from everyone's sections and submits it with the zip | A new `scripts/plot_results.py`, plus `results/` and the report | The chart and table cover all three structures at all three sizes, and the report is submitted | Chart Nov 15, report Nov 29 |

Heidtmann can start the chart script now. `results/first-run-2026-11-04-sorted-vector.csv` has the same columns `./atc bench` writes, so the script can be built on that file before the heaps are done.

Each of us writes the report section and makes the slides for our own part:
- William: the application and dataset, and Structure A.
- Kyle: Structure B.
- Pau: the indexed heap research.
- Heidtmann: the experiments (how we tested, the table and the chart) and the title page.
- All of us: the conclusions.

The comments at the top of `LazyHeapQueue.h` and `IndexedHeapQueue.h` describe the plan for each heap.

## Timeline

| Date | What |
|---|---|
| **Thu Nov 5, 11:59 PM** | Progress report on Canvas. It isn't set up as a group assignment, so each of us uploads it |
| Thu Nov 12 | Both heaps pass `make test` and `./atc check` (Kyle, Pau) |
| Sun Nov 15 | First full benchmark with all three structures; chart script works (Heidtmann) |
| Sun Nov 22 | Everyone's report section and slides drafted; research write-up done (Test 3 is Tue Nov 24) |
| Sun Nov 29 | Report put together (Heidtmann), deck combined (William), one practice run of the 5-minute talk |
| **Tue Dec 1** | Presentation in class. Submit the report and a zip of this repo (GitHub → Code → Download ZIP) by midnight. Canvas says Dec 1 and the handout says Dec 2, so we aim for Dec 1 |

## Getting started

You need g++ with C++17, make and git. On Windows, use WSL, or MinGW with the commands under "Without make".

```sh
git clone https://github.com/adainstarks/atc-sim.git
cd atc-sim
make            # builds ./atc and ./test_queues
make test       # unit tests; an unfinished structure shows SKIP
./atc demo      # lands the first 1,000 flights and prints the first landings
./atc check     # each finished structure must land every flight in the real order
./atc bench     # times each finished structure at 1,000, 10,000 and 100,000 flights
```

Run `./atc` from the repo folder, since it reads `data/atl_arrivals.csv`.

Without make:

```sh
g++ -std=c++17 -O2 -Isrc src/*.cpp -o atc
g++ -std=c++17 -O2 -Isrc tests/test_queues.cpp src/Flight.cpp src/Simulation.cpp src/SortedVectorQueue.cpp src/LazyHeapQueue.cpp src/IndexedHeapQueue.cpp -o test_queues
```

## How the simulation works

Each flight's id is its row number in `data/atl_arrivals.csv`. Its key is its arrival time in minutes since May 1, 2026, 00:00 Atlanta time. Flights due the same minute land in id order, so every structure must produce exactly the same landing order.

One run on the first N flights has four phases, each timed separately:

1. **Add.** All N flight plans join the list in a shuffled order. The seed is fixed, so every structure and every run gets the same order.
2. **Delay.** Every flight that landed early or late moves to its real arrival time. That's 98.6% of the flights that landed.
3. **Remove.** Cancelled and diverted flights leave without landing.
4. **Land.** The earliest flight lands, over and over, until the list is empty.

After the run, outside the timing, the landing order is compared with the order the planes really landed in. A structure that gets it wrong is flagged.

Only the calls to the structure are timed. Loading the CSV and working out the schedule happen before the clock starts. `bench` repeats each size 5 times and writes every run to `results/results.csv`.

## The interface

Every structure is a class derived from `ArrivalQueue` in `src/ArrivalQueue.h`:

| Function | What it does |
|---|---|
| `addFlight(id, arrival)` | A flight joins the list |
| `delayFlight(id, newArrival)` | Its arrival time changes, later or earlier |
| `removeFlight(id)` | Cancelled or diverted: it leaves without landing |
| `landNext(id, arrival)` | Removes the earliest flight and reports it. Returns false if the list is empty |
| `size()` | How many flights are still waiting |

Ids run from 0 to `maxFlights - 1`, the number passed to the constructor. Each id is added once. `delayFlight` and `removeFlight` are only called for flights still in the list.

The unfinished functions throw `NotWrittenYet`. The tests and the benchmark skip anything that still throws, so the program builds and runs before everyone's part is done. To finish a structure, replace the throws with real code. Don't change the public functions, because the simulator, tests and benchmark all call them.

## Big-O predictions

n is the number of flights in the list.

| Operation | A: sorted vector | B: heap with lazy deletion | C: indexed heap |
|---|---|---|---|
| Land next | O(1): `pop_back` | O(log n) amortized, including popping stale copies | O(log n) |
| Add | O(n): binary search, then `insert` shifts entries | O(log n) | O(log n) |
| Delay | O(n): erase and insert again | O(log n): push a new copy; the old one goes stale | O(log n): sift up or down in place |
| Remove | O(n): `erase` shifts entries | O(1): mark it gone | O(log n) |
| A whole run of n flights | O(n²) | O(n log n) | O(n log n) |

Our prediction: the heaps win at large n, because every add and delay in the vector shifts O(n) entries. The vector stays competitive at small n and always lands fastest.

## Results

Final benchmark: 3 structures x 3 sizes x 5 runs (45 runs, all in `results/results.csv`), measured on Linux x86_64 with g++ 14.2.0, `-O2`. Times are averages in milliseconds; lower is better. Charts and `benchmark_summary.csv` are in `results/charts/` (made by `scripts/plot_results.py`).

| Average total time (ms) | 1,000 | 10,000 | 100,000 |
|---|---|---|---|
| sorted-vector | 0.231 | 13.063 | 1426.500 |
| lazy-heap | 0.106 | 1.765 | 29.345 |
| indexed-heap | 0.059 | 0.910 | 14.036 |

At 100,000 flights:

| Structure | Add | Delay | Remove | Land | Total |
|---|---|---|---|---|---|
| sorted-vector | 303.846 | 1107.514 | 14.548 | 0.594 | 1426.500 |
| lazy-heap | 3.342 | 2.558 | 0.011 | 23.434 | 29.345 |
| indexed-heap | 2.178 | 0.883 | 0.047 | 10.928 | 14.036 |

- **The indexed heap was fastest at every size**, about 100x faster than the sorted vector and about 2x faster than the lazy heap at 100,000 flights.
- **The prediction held for landing and for large n.** The vector lands fastest (0.59 ms for 97,783 landings), and its total grew about 109x from 10,000 to 100,000 flights, close to the 100x expected from O(n²).
- **One prediction did not hold.** The vector was not competitive even at 1,000 flights (0.231 ms against 0.059 ms for the indexed heap).
- **Limits:** one machine, the first N flights only, no confidence intervals. Rerun `./atc bench` on your own computer to check the ranking; exact times will differ.

The earlier November 4 measurement of the sorted vector alone (Intel i5-1334U, 5 runs per size) is kept in `results/first-run-2026-11-04-sorted-vector.csv` as a baseline. It is not mixed into the numbers above.

The full write-up is in the final report, submitted with this repo. The indexed heap research is in `docs/research-indexed-heap.md`.

## The data

The data is the Bureau of Transportation Statistics' Airline On-Time Performance data, May–August 2026. It has every domestic flight by the major U.S. airlines that was scheduled to land at ATL, sorted by scheduled arrival:
- 109,781 flights in all
- 107,500 landed: 66% early, 33% late
- 1,781 cancelled and 500 diverted

The benchmark sizes are the first 1,000 rows (about May 1–2), the first 10,000 (through May 12) and the first 100,000 (through August 20). `data/README.md` has the citation, the columns and how the file was made.

## Working together

First accept your invite. GitHub emails it, or open https://github.com/adainstarks/atc-sim/invitations. Then work on your own branch:

```sh
git checkout -b kyle/lazy-heap
# edit src/LazyHeapQueue.h and src/LazyHeapQueue.cpp
make test && ./atc check
git add src/LazyHeapQueue.h src/LazyHeapQueue.cpp
git commit -m "Lazy heap: addFlight and landNext"
git push -u origin kyle/lazy-heap
```

Then open a pull request on GitHub, and William merges it. GitHub builds the code and runs the tests on every push; a red X means something broke.

No invite yet? Click **Fork**, push your branch to your fork, and open the pull request from there. The first time, William has to click "Approve and run" before the tests start.

- Edit only your own files. If you need a change to a shared file (`ArrivalQueue.h`, `Simulation.*` or the tests), say so in the group chat first, because it affects everyone.
- Don't commit build output (`atc`, `test_queues`, `*.o`). `.gitignore` already skips it.

## AI use

The handout allows AI on this project only, for code, debugging, data preparation, testing and graphing. These rules come with it:
- **Disclose meaningful AI help.** Fill in the A.I. Disclaimer at the top of each file you write, and add a line to `docs/ai-use-log.md`.
- **Check what it gives you.** `make test` and `./atc check` must pass.
- **Be able to explain your part.** Dr. Li can ask any of us about any of it.
- **Never report a measurement we didn't actually run.**

## Repo layout

```
src/ArrivalQueue.h            the interface, Entry (arrival, id) and NotWrittenYet
src/Flight.h, Flight.cpp      one flight, and the CSV loader
src/Simulation.h, .cpp        builds the schedule from the data; runs and times the 4 phases
src/SortedVectorQueue.h, .cpp Structure A (William)
src/LazyHeapQueue.h, .cpp     Structure B (Kyle)
src/IndexedHeapQueue.h, .cpp  Structure C, research (Pau)
src/main.cpp                  ./atc demo | check | bench
tests/test_queues.cpp         unit tests, run on every structure
scripts/prepare_data.py       rebuilds the data from BTS (not needed: the CSV is committed)
data/                         atl_arrivals.csv, plus a README with the source and columns
results/                      benchmark output
docs/ai-use-log.md            our AI disclosure log
```
