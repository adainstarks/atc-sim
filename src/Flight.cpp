// A.I. Disclaimer: written with help from Claude (Anthropic), an AI assistant. The semester
// project allows AI with disclosure; the team checks this code with the tests in tests/.
/*
 PROGRAMMER:          William Starks
 Program Description: Reads data/atl_arrivals.csv into a vector of Flight.
*/
#include "Flight.h"

#include <cstdio>
#include <fstream>
#include <sstream>
#include <stdexcept>

static const std::string HEADER =
    "FlightDate,Reporting_Airline,Flight_Number_Reporting_Airline,Origin,"
    "CRSArrTime,ArrTime,ArrDelay,Cancelled,Diverted,SchedArrMin";

// Reads one line, without the '\r' a Windows checkout may leave on the end.
static bool readLine(std::istream &in, std::string &line)
{
    if (!std::getline(in, line))
        return false;
    if (!line.empty() && line.back() == '\r')
        line.pop_back();
    return true;
}

// Splits a line on commas. The file has no quoted fields; blank fields stay as "".
static std::vector<std::string> splitLine(const std::string &line)
{
    std::vector<std::string> fields;
    std::stringstream ss(line);
    std::string field;
    while (std::getline(ss, field, ','))
        fields.push_back(field);
    if (!line.empty() && line.back() == ',')
        fields.push_back("");
    return fields;
}

// A blank field (a flight that never arrived has no ArrDelay) counts as 0.
static int toInt(const std::string &s, int lineNo)
{
    if (s.empty())
        return 0;
    try
    {
        return std::stoi(s);
    }
    catch (const std::exception &)
    {
        throw std::runtime_error("line " + std::to_string(lineNo) + ": not a number: " + s);
    }
}

std::vector<Flight> loadFlights(const std::string &path, int maxRows)
{
    std::ifstream in(path);
    if (!in)
        throw std::runtime_error("can't open " + path + " (run the program from the repo folder)");

    std::string line;
    if (!readLine(in, line) || line != HEADER)
        throw std::runtime_error(path + " doesn't look like the output of scripts/prepare_data.py");

    std::vector<Flight> flights;
    int lineNo = 1;
    while ((maxRows <= 0 || (int)flights.size() < maxRows) && readLine(in, line))
    {
        lineNo++;
        if (line.empty())
            continue;
        std::vector<std::string> f = splitLine(line);
        if (f.size() != 10)
            throw std::runtime_error(path + " line " + std::to_string(lineNo) + ": expected 10 fields");

        Flight flight;
        flight.date = f[0];
        flight.airline = f[1];
        flight.number = toInt(f[2], lineNo);
        flight.origin = f[3];
        flight.arrDelay = toInt(f[6], lineNo);
        flight.cancelled = f[7] == "1";
        flight.diverted = f[8] == "1";
        flight.schedArrival = toInt(f[9], lineNo);
        if (!flight.lands())
            flight.arrDelay = 0;
        flights.push_back(flight);
    }
    return flights;
}

std::string clockTime(int minutes)
{
    int m = (minutes % 1440 + 1440) % 1440;
    char text[16];
    std::snprintf(text, sizeof text, "%02d:%02d", m / 60, m % 60);
    return text;
}
