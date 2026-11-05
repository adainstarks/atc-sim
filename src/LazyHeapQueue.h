// A.I. Disclaimer: TODO (owner) - say whether you used AI on this file, and for what.
/*
 PROGRAMMER:          Kyle Penton
 Program Description: Structure B. The arrival list as a binary min-heap with lazy deletion.
*/
#ifndef LAZYHEAPQUEUE_H
#define LAZYHEAPQUEUE_H

#include "ArrivalQueue.h"

#include <functional>
#include <queue>
#include <vector>

// Structure B: a binary min-heap (std::priority_queue) of Entry, earliest flight on top.
//
// The plan from our proposal:
//   addFlight     push the flight                                              O(log n)
//   landNext      pop the top, after throwing away out-of-date copies on top   O(log n) amortized
//   delayFlight   push a NEW copy with the new time. The old copy stays in     O(log n)
//                 the heap (a heap can't find it quickly) and is now stale.
//   removeFlight  remember the flight is gone, so its copy is stale too        O(1)
//
// landNext needs a way to tell a stale copy from the live one. Hint: remember each
// flight's current arrival time by id. size() is the number of flights waiting, not
// the number of entries in the heap, which includes stale copies.
//
// Don't change the public functions; the simulator, tests and benchmark call them.
// `make test` checks your work, including a flight delayed and then moved back to its
// original time.
class LazyHeapQueue : public ArrivalQueue
{
public:
    explicit LazyHeapQueue(int maxFlights);
    std::string name() const { return "lazy-heap"; }
    void addFlight(int id, int arrival);
    void delayFlight(int id, int newArrival);
    void removeFlight(int id);
    bool landNext(int &id, int &arrival);
    int size() const;

private:
    // TODO: your data members, for example
    //   std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>> heap;
};

#endif
