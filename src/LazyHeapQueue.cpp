// A.I. Disclaimer: AI (Claude) was used ONLY for debugging purposes.
/*
 PROGRAMMER:          Kyle Penton
 Program Description: Structure B, the lazy-deletion heap (see LazyHeapQueue.h).
*/
#include "LazyHeapQueue.h"

// Each function below throws NotWrittenYet until it is written. Replace the throw with
// the real code; the tests skip whatever still throws.

LazyHeapQueue::LazyHeapQueue(int maxFlights)
    : arrivalOf(maxFlights, GONE), liveCount(0)
{
}

bool LazyHeapQueue::isWaiting(int id) const
{
    return id >= 0 && id < static_cast<int>(arrivalOf.size()) && arrivalOf[id] != GONE;
}

// A copy is current only if it matches the flight's recorded arrival time.
bool LazyHeapQueue::isLive(const Entry &e) const
{
    return isWaiting(e.id) && arrivalOf[e.id] == e.arrival;
}

// Pop out-of-date copies off the top. Each copy is popped at most once, so landNext
// is O(log n) amortized.
void LazyHeapQueue::discardStaleTop()
{
    while (!heap.empty() && !isLive(heap.top()))
        heap.pop();
}

void LazyHeapQueue::addFlight(int id, int arrival)
{
    if (id >= static_cast<int>(arrivalOf.size()))
        arrivalOf.resize(id + 1, GONE);

    if (arrivalOf[id] == GONE)
        ++liveCount;
    arrivalOf[id] = arrival;

    Entry e = {arrival, id};
    heap.push(e);  // O(log n)
}

void LazyHeapQueue::delayFlight(int id, int newArrival)
{
    if (!isWaiting(id))
        throw std::invalid_argument("LazyHeapQueue: flight " + std::to_string(id)
                                    + " is not in the list");
    if (arrivalOf[id] == newArrival)
        return;  // time unchanged, nothing to do

    arrivalOf[id] = newArrival;  // the old copy is now stale
    Entry e = {newArrival, id};
    heap.push(e);  // O(log n)
}

void LazyHeapQueue::removeFlight(int id)
{
    if (!isWaiting(id))
        throw std::invalid_argument("LazyHeapQueue: flight " + std::to_string(id)
                                    + " is not in the list");
    arrivalOf[id] = GONE;  // O(1): every copy is now stale
    --liveCount;
}

bool LazyHeapQueue::landNext(int &id, int &arrival)
{
    discardStaleTop();
    if (heap.empty())
        return false;  // changes nothing, as the interface requires

    Entry e = heap.top();
    heap.pop();
    id = e.id;
    arrival = e.arrival;

    arrivalOf[id] = GONE;  // also makes any identical duplicate stale
    --liveCount;
    return true;
}

int LazyHeapQueue::size() const
{
    return liveCount;
}
}
