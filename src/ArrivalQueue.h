// A.I. Disclaimer: written with help from Claude (Anthropic), an AI assistant. The semester
// project allows AI with disclosure; the team checks this code with the tests in tests/.
/*
 PROGRAMMER:          William Starks
 Program Description: The interface every arrival-list structure implements, so the
                      simulator, the tests and the benchmark can run any of them the same way.
*/
#ifndef ARRIVALQUEUE_H
#define ARRIVALQUEUE_H

#include <stdexcept>
#include <string>

// One flight waiting in a list: when it arrives, and which flight it is.
// Flights land earliest first. Two flights due the same minute land in id order, so every
// structure produces exactly the same landing order.
struct Entry
{
    int arrival;  // minutes since May 1, 2026 00:00, Atlanta time
    int id;       // the flight's row number in data/atl_arrivals.csv
};

inline bool operator<(const Entry &a, const Entry &b)
{
    if (a.arrival != b.arrival)
        return a.arrival < b.arrival;
    return a.id < b.id;
}

inline bool operator>(const Entry &a, const Entry &b)
{
    return b < a;
}

inline bool operator==(const Entry &a, const Entry &b)
{
    return a.arrival == b.arrival && a.id == b.id;
}

// Thrown by a function nobody has written yet. The tests and the benchmark skip a
// structure that throws this instead of crashing.
class NotWrittenYet : public std::logic_error
{
public:
    explicit NotWrittenYet(const std::string &function)
        : std::logic_error(function + " is not written yet") {}
};

// The tower's arrival list. Flight ids run from 0 to maxFlights-1, where maxFlights is
// passed to each structure's constructor. Each id is added at most once, and the simulator
// only delays or removes flights that are still in the list.
class ArrivalQueue
{
public:
    virtual ~ArrivalQueue() {}

    virtual std::string name() const = 0;

    // A new flight joins the list.
    virtual void addFlight(int id, int arrival) = 0;

    // A flight's arrival time changes, later (delayed) or earlier (early). It must land in
    // its new place in the order.
    virtual void delayFlight(int id, int newArrival) = 0;

    // A cancelled or diverted flight leaves the list without landing.
    virtual void removeFlight(int id) = 0;

    // Lands the earliest flight: removes it and reports it through id and arrival.
    // Returns false, and changes nothing, if the list is empty.
    virtual bool landNext(int &id, int &arrival) = 0;

    // How many flights are still waiting to land.
    virtual int size() const = 0;
};

#endif
