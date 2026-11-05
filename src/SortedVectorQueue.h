// A.I. Disclaimer: written with help from Claude (Anthropic), an AI assistant. The semester
// project allows AI with disclosure; the team checks this code with the tests in tests/.
/*
 PROGRAMMER:          William Starks
 Program Description: Structure A. The arrival list as a vector kept sorted latest-first,
                      so the next flight to land is always at the back.
*/
#ifndef SORTEDVECTORQUEUE_H
#define SORTEDVECTORQUEUE_H

#include "ArrivalQueue.h"

#include <vector>

// Structure A: a sorted vector, earliest flight at the back.
//
//   landNext      pop_back                                        O(1)
//   addFlight     binary search for the spot O(log n), then insert shifts the entries after it   O(n)
//   delayFlight   remove the old entry, add it back with the new time                           O(n)
//   removeFlight  binary search, then erase shifts the entries after it                         O(n)
class SortedVectorQueue : public ArrivalQueue
{
public:
    explicit SortedVectorQueue(int maxFlights);
    std::string name() const { return "sorted-vector"; }
    void addFlight(int id, int arrival);
    void delayFlight(int id, int newArrival);
    void removeFlight(int id);
    bool landNext(int &id, int &arrival);
    int size() const;

private:
    std::vector<Entry> list;     // sorted latest-first: list.back() is the next flight to land
    std::vector<int> arrivalOf;  // arrivalOf[id] = that flight's arrival while it is in the list

    // Binary search: the first position whose entry is not later than e. That is where e
    // sits if it is in the list, or where it belongs if it isn't.
    std::vector<Entry>::iterator findSpot(const Entry &e);
};

#endif
