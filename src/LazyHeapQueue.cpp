// A.I. Disclaimer: AI (Claude) was used ONLY for debugging purposes.
/*
 PROGRAMMER:          Kyle Penton
 Program Description: Structure B, the lazy-deletion heap (see LazyHeapQueue.h).
*/
#include "LazyHeapQueue.h"
#include <stdexcept>
#include <string>

LazyHeapQueue::LazyHeapQueue(int maxFlights)
    : arrivalOf(maxFlights, -1), liveCount(0)
{
}

// A copy in the heap is current only if it matches the flight's recorded arrival time.
bool LazyHeapQueue::isLive(const Entry &e) const
{
    return e.id >= 0 && e.id < static_cast<int>(arrivalOf.size())
        && arrivalOf[e.id] == e.arrival;
}

// Throw away out-of-date copies sitting on top. Each stale copy is popped at most once
// in its lifetime, which is why landNext is O(log n) amortized.
void LazyHeapQueue::discardStaleTop()
{
    while (!heap.empty() && !isLive(heap.top()))
        heap.pop();
}

void LazyHeapQueue::addFlight(int id, int arrival)
{
    if (id >= static_cast<int>(arrivalOf.size()))
        arrivalOf.resize(id + 1, -1);

    if (arrivalOf[id] == -1)        // a brand-new flight, not a re-add of a waiting one
        ++liveCount;
    arrivalOf[id] = arrival;

    Entry e = {arrival, id};
    heap.push(e);                   // O(log n)
}

void LazyHeapQueue::delayFlight(int id, int newArrival)
{
    if (id < 0 || id >= static_cast<int>(arrivalOf.size()) || arrivalOf[id] == -1)
        throw std::invalid_argument("LazyHeapQueue: flight " + std::to_string(id)
                                    + " is not in the list");

    // The old copy stays in the heap and becomes stale because arrivalOf changes.
    arrivalOf[id] = newArrival;
    Entry e = {newArrival, id};
    heap.push(e);                   // O(log n); liveCount is unchanged
}

void LazyHeapQueue::removeFlight(int id)
{
    if (id < 0 || id >= static_cast<int>(arrivalOf.size()) || arrivalOf[id] == -1)
        throw std::invalid_argument("LazyHeapQueue: flight " + std::to_string(id)
                                    + " is not in the list");

    arrivalOf[id] = -1;             // O(1): every copy of this flight is now stale
    --liveCount;
}

bool LazyHeapQueue::landNext(int &id, int &arrival)
{
    discardStaleTop();
    if (heap.empty())
        return false;

    Entry e = heap.top();
    heap.pop();

    id = e.id;
    arrival = e.arrival;

    // Marking the flight gone also makes any duplicate copy stale (see below).
    arrivalOf[id] = -1;
    --liveCount;
    return true;
}

int LazyHeapQueue::size() const
{
    return liveCount;
}
