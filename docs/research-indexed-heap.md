# Research: the indexed heap (Structure C)

Section author: Pau Mang. Written with help from Claude (Anthropic) from the code in `src/IndexedHeapQueue.*` and the measurements in `results/results.csv`, as disclosed in `docs/ai-use-log.md`.

## The problem a plain heap has

A binary heap can find the smallest entry instantly, but it cannot find an entry in the middle. A flight's arrival time changes for 98.6% of landed flights, and cancelled flights must leave. The lazy heap (Structure B) gets around this by pushing a new copy and leaving the old one to be thrown away later. That keeps every operation cheap but fills the heap with dead entries.

## How the indexed heap works

The indexed heap (also called an addressable priority queue; Sedgewick and Wayne's *Algorithms* has a version called `IndexMinPQ`) keeps the heap in a vector of `Entry` and adds one array, `pos_`, where `pos_[id]` is that flight's current index in the heap, or -1 if it is not in the list. Every swap inside the heap goes through `swapAt`, which updates `pos_` for both entries. So the heap can always answer "where is flight 7?" in O(1).

| Operation | What it does | Cost |
|---|---|---|
| `addFlight` | append at the end, sift up | O(log n) |
| `delayFlight` | look up the index in `pos_`, change the time, sift up if the entry got smaller, sift down if larger | O(log n) |
| `removeFlight` | move the last entry into the hole, shrink the vector, sift up or down | O(log n) |
| `landNext` | remove the entry at index 0 the same way | O(log n) |

There are no stale copies. The heap holds exactly the flights that are waiting, and `size()` is just the vector's size.

## Does it beat lazy deletion on our data?

Yes. Average total time over 5 runs:

| Flights | Lazy heap | Indexed heap | Indexed is faster by |
|---|---|---|---|
| 1,000 | 0.106 ms | 0.059 ms | 1.8x |
| 10,000 | 1.765 ms | 0.910 ms | 1.9x |
| 100,000 | 29.345 ms | 14.036 ms | 2.1x |

At 100,000 flights, delay was 2.9x faster (0.883 vs 2.558 ms) and landing 2.1x faster (10.928 vs 23.434 ms). The lazy heap's heap reaches about 196,000 entries at that size (100,000 adds plus 96,446 delays), roughly twice the number of live flights, and `landNext` has to pop the dead ones too. The indexed heap's heap never holds more than 100,000.

The lazy heap wins in one place: removal. Marking a flight gone is O(1) (0.011 ms) against O(log n) for the indexed heap (0.047 ms). Removal is under 0.4% of the indexed heap's total on this data, so it does not change the ranking. A workload with many more cancellations could narrow the gap.

## Cost of the indexed heap

It needs the extra `pos_` array and a little more bookkeeping on every swap, and it is harder to write correctly than the lazy heap. Our 12,000-operation random test against a reference, plus the full-dataset landing-order check, is what gives us confidence in it.

*Limits:* one machine, one compiler, the first N flights of one data set, no confidence intervals. See the experiments section of the report.
