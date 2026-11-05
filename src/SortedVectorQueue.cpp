// A.I. Disclaimer: written with help from Claude (Anthropic), an AI assistant. The semester
// project allows AI with disclosure; the team checks this code with the tests in tests/.
/*
 PROGRAMMER:          William Starks
 Program Description: Structure A, the sorted vector (see SortedVectorQueue.h).
*/
#include "SortedVectorQueue.h"

#include <algorithm>
#include <functional>

SortedVectorQueue::SortedVectorQueue(int maxFlights) : arrivalOf(maxFlights, -1)
{
    list.reserve(maxFlights);
}

std::vector<Entry>::iterator SortedVectorQueue::findSpot(const Entry &e)
{
    // The list is in descending order, so the search compares with > instead of <.
    return std::lower_bound(list.begin(), list.end(), e, std::greater<Entry>());
}

void SortedVectorQueue::addFlight(int id, int arrival)
{
    Entry e = {arrival, id};
    list.insert(findSpot(e), e);  // every entry after the spot moves over one place
    arrivalOf[id] = arrival;
}

void SortedVectorQueue::delayFlight(int id, int newArrival)
{
    removeFlight(id);
    addFlight(id, newArrival);
}

void SortedVectorQueue::removeFlight(int id)
{
    Entry e = {arrivalOf[id], id};
    std::vector<Entry>::iterator spot = findSpot(e);
    if (spot == list.end() || !(*spot == e))
        throw std::invalid_argument("SortedVectorQueue: flight " + std::to_string(id) + " is not in the list");
    list.erase(spot);  // every entry after it moves back one place
    arrivalOf[id] = -1;
}

bool SortedVectorQueue::landNext(int &id, int &arrival)
{
    if (list.empty())
        return false;
    id = list.back().id;
    arrival = list.back().arrival;
    list.pop_back();
    arrivalOf[id] = -1;
    return true;
}

int SortedVectorQueue::size() const
{
    return (int)list.size();
}
