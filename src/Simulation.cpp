// A.I. Disclaimer: written with help from Claude (Anthropic), an AI assistant. The semester
// project allows AI with disclosure; the team checks this code with the tests in tests/.
/*
 PROGRAMMER:          William Starks
 Program Description: The simulator (see Simulation.h).
*/
#include "Simulation.h"

#include "IndexedHeapQueue.h"
#include "LazyHeapQueue.h"
#include "SortedVectorQueue.h"

#include <algorithm>
#include <chrono>
#include <random>
#include <stdexcept>

const std::vector<std::string> &structureNames()
{
    static const std::vector<std::string> names = {"sorted-vector", "lazy-heap", "indexed-heap"};
    return names;
}

std::unique_ptr<ArrivalQueue> makeQueue(const std::string &name, int maxFlights)
{
    if (name == "sorted-vector")
        return std::make_unique<SortedVectorQueue>(maxFlights);
    if (name == "lazy-heap")
        return std::make_unique<LazyHeapQueue>(maxFlights);
    if (name == "indexed-heap")
        return std::make_unique<IndexedHeapQueue>(maxFlights);
    throw std::invalid_argument("unknown structure: " + name);
}

bool isWritten(const std::string &name, std::string &missing)
{
    try
    {
        std::unique_ptr<ArrivalQueue> q = makeQueue(name, 3);
        int id, arrival;
        q->addFlight(0, 10);
        q->addFlight(1, 20);
        q->addFlight(2, 30);
        q->delayFlight(0, 25);
        q->removeFlight(2);
        q->size();
        q->landNext(id, arrival);
    }
    catch (const NotWrittenYet &e)
    {
        missing = e.what();
        return false;
    }
    catch (const std::exception &)
    {
        // Written, but broken. The order check and the tests report how.
    }
    return true;
}

Schedule makeSchedule(const std::vector<Flight> &flights)
{
    Schedule s;
    int n = flights.size();

    // Flight plans reach the tower in no particular order, so shuffle the ids. The fixed
    // seed gives every structure and every run the same order. (Fisher-Yates by hand,
    // because std::shuffle gives different orders with different compilers.)
    std::vector<int> order(n);
    for (int i = 0; i < n; i++)
        order[i] = i;
    std::mt19937 rng(3110);
    for (int i = n - 1; i > 0; i--)
        std::swap(order[i], order[rng() % (i + 1)]);
    for (int id : order)
        s.plans.push_back({flights[id].schedArrival, id});

    // Delays and removals come in the order the flights were due.
    std::vector<Entry> landed;
    for (int id = 0; id < n; id++)
    {
        if (!flights[id].lands())
        {
            s.removals.push_back(id);
            continue;
        }
        if (flights[id].arrDelay != 0)
            s.changes.push_back({flights[id].actualArrival(), id});
        landed.push_back({flights[id].actualArrival(), id});
    }

    std::sort(landed.begin(), landed.end());
    for (const Entry &e : landed)
        s.expectedOrder.push_back(e.id);
    return s;
}

static double millisecondsBetween(std::chrono::steady_clock::time_point start,
                                  std::chrono::steady_clock::time_point end)
{
    return std::chrono::duration<double, std::milli>(end - start).count();
}

RunResult runSimulation(ArrivalQueue &q, const Schedule &s)
{
    typedef std::chrono::steady_clock Clock;
    RunResult r;
    r.landingOrder.reserve(s.plans.size());

    // Phase 1: every flight plan joins the arrival list.
    Clock::time_point t0 = Clock::now();
    for (const Entry &e : s.plans)
        q.addFlight(e.id, e.arrival);

    // Phase 2: flights that ran early or late move to their real arrival time.
    Clock::time_point t1 = Clock::now();
    for (const Entry &e : s.changes)
        q.delayFlight(e.id, e.arrival);

    // Phase 3: cancelled and diverted flights leave without landing.
    Clock::time_point t2 = Clock::now();
    for (int id : s.removals)
        q.removeFlight(id);

    // Phase 4: land everything, earliest first.
    Clock::time_point t3 = Clock::now();
    int id, arrival;
    while (q.landNext(id, arrival))
        r.landingOrder.push_back(id);
    Clock::time_point t4 = Clock::now();

    r.addMs = millisecondsBetween(t0, t1);
    r.delayMs = millisecondsBetween(t1, t2);
    r.removeMs = millisecondsBetween(t2, t3);
    r.landMs = millisecondsBetween(t3, t4);
    return r;
}
