// A.I. Disclaimer: TODO (owner) - say whether you used AI on this file, and for what.
/*
 PROGRAMMER:          Pau Mang
 Program Description: Structure C, our independent research. The arrival list as an
                      indexed binary min-heap.
*/
#ifndef INDEXEDHEAPQUEUE_H
#define INDEXEDHEAPQUEUE_H

#include "ArrivalQueue.h"

#include <vector>

// Structure C: an indexed binary min-heap, written by hand (not std::priority_queue).
//
// The heap lives in a vector<Entry>: root at index 0, children of i at 2i+1 and 2i+2.
// A position table records where each flight sits: pos[id] = its index in the heap
// vector, or -1 if it isn't in the heap. Every swap must update pos for both flights.
//
//   addFlight     put it at the end, sift up                                     O(log n)
//   landNext      take the root, move the last entry to the root, sift down       O(log n)
//   delayFlight   change the time in place at pos[id], then sift up if it got     O(log n)
//                 earlier or down if it got later
//   removeFlight  move the last entry into its spot, shrink, then sift that       O(log n)
//                 entry up or down
//
// Unlike lazy-heap there are no stale copies. Our research question: is that worth the
// extra bookkeeping on real data? The benchmark answers it.
//
// Don't change the public functions; the simulator, tests and benchmark call them.
// `make test` checks your work.
class IndexedHeapQueue : public ArrivalQueue
{
public:
    explicit IndexedHeapQueue(int maxFlights);
    std::string name() const { return "indexed-heap"; }
    void addFlight(int id, int arrival);
    void delayFlight(int id, int newArrival);
    void removeFlight(int id);
    bool landNext(int &id, int &arrival);
    int size() const;

private:
    // TODO: your data members and helpers, for example
    std::vector<Entry> heap;
    std::vector<int> pos;
    void siftUp(int i);
    void siftDown(int i);
};

#endif
