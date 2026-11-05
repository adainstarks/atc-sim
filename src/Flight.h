// A.I. Disclaimer: written with help from Claude (Anthropic), an AI assistant. The semester
// project allows AI with disclosure; the team checks this code with the tests in tests/.
/*
 PROGRAMMER:          William Starks
 Program Description: One real flight from the BTS data, and the loader that reads
                      data/atl_arrivals.csv (made by scripts/prepare_data.py).
*/
#ifndef FLIGHT_H
#define FLIGHT_H

#include <string>
#include <vector>

// One row of data/atl_arrivals.csv: a flight scheduled to land in Atlanta.
struct Flight
{
    std::string date;     // FlightDate: the day it departed, e.g. 2026-05-01
    std::string airline;  // Reporting_Airline, e.g. DL
    int number;           // Flight_Number_Reporting_Airline
    std::string origin;   // Origin airport, e.g. DEN
    int schedArrival;     // SchedArrMin: scheduled arrival, minutes since May 1, 2026 00:00 Atlanta time
    int arrDelay;         // ArrDelay: minutes late (negative = early); 0 if it never arrived
    bool cancelled;
    bool diverted;

    bool lands() const { return !cancelled && !diverted; }
    int actualArrival() const { return schedArrival + arrDelay; }
};

// Reads the first maxRows flights (every flight if maxRows is 0). Flight i of the result
// has id i. Throws std::runtime_error if the file is missing or malformed.
std::vector<Flight> loadFlights(const std::string &path, int maxRows = 0);

// "05:49" for a time given in minutes since midnight of the first day.
std::string clockTime(int minutes);

#endif
