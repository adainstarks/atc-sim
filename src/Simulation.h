// A.I. Disclaimer: written with help from Claude (Anthropic), an AI assistant. The semester
// project allows AI with disclosure; the team checks this code with the tests in tests/.
/*
 PROGRAMMER:          William Starks
 Program Description: Turns the flight data into the tower's schedule of operations and
                      runs it against any arrival-list structure, timing each phase.
*/
#ifndef SIMULATION_H
#define SIMULATION_H

#include "ArrivalQueue.h"
#include "Flight.h"

#include <memory>
#include <string>
#include <vector>

// The structures we compare, in the order they are listed in output.
const std::vector<std::string> &structureNames();

// A new, empty structure, by name, for flight ids 0 to maxFlights-1.
std::unique_ptr<ArrivalQueue> makeQueue(const std::string &name, int maxFlights);

// False if any function of that structure still throws NotWrittenYet; `missing` says which.
bool isWritten(const std::string &name, std::string &missing);

// Everything the tower will be told, worked out from the data before any timing starts.
struct Schedule
{
    std::vector<Entry> plans;        // phase 1: (scheduled arrival, id) in filing order
    std::vector<Entry> changes;      // phase 2: (actual arrival, id) for flights that landed early or late
    std::vector<int> removals;       // phase 3: ids of cancelled and diverted flights
    std::vector<int> expectedOrder;  // the order the flights must land in: the real arrival order
};

Schedule makeSchedule(const std::vector<Flight> &flights);

// What one run did, and how long each phase took in milliseconds.
struct RunResult
{
    double addMs, delayMs, removeMs, landMs;
    std::vector<int> landingOrder;  // flight ids in the order they landed

    double totalMs() const { return addMs + delayMs + removeMs + landMs; }
};

// Runs the four phases on an empty structure. Only the calls to the structure are timed.
RunResult runSimulation(ArrivalQueue &q, const Schedule &s);

#endif
