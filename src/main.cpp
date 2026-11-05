// A.I. Disclaimer: written with help from Claude (Anthropic), an AI assistant. The semester
// project allows AI with disclosure; the team checks this code with the tests in tests/.
/*
 PROGRAMMER:          William Starks
 Program Description: Air traffic control simulator, command-line driver.
   Input:  data/atl_arrivals.csv, real Atlanta arrivals from the BTS (see data/README.md).
   Output: demo prints a landing log; check verifies each structure lands flights in the
           real arrival order; bench times each structure at three sizes and writes
           results/results.csv.
*/
#include "Simulation.h"

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>

static const std::string DATA = "data/atl_arrivals.csv";
static const std::string RESULTS = "results/results.csv";

static void usage()
{
    std::cout << "usage: ./atc demo  [flights] [structure]  land a schedule and print the first landings\n"
                 "       ./atc check [flights]              check each finished structure lands in the real order\n"
                 "       ./atc bench [runs]                 time each finished structure at 1,000, 10,000\n"
                 "                                          and 100,000 flights (default 5 runs each)\n"
                 "structures: sorted-vector, lazy-heap, indexed-heap\n";
}

// Where two landing orders first differ, in words.
static std::string firstDifference(const std::vector<int> &got, const std::vector<int> &want)
{
    for (size_t i = 0; i < got.size() && i < want.size(); i++)
        if (got[i] != want[i])
            return "landing #" + std::to_string(i + 1) + " was flight " + std::to_string(got[i]) +
                   ", should be flight " + std::to_string(want[i]);
    return "landed " + std::to_string(got.size()) + " flights, should be " + std::to_string(want.size());
}

static int demo(int n, const std::string &name)
{
    std::vector<Flight> flights = loadFlights(DATA, n);
    Schedule s = makeSchedule(flights);
    std::unique_ptr<ArrivalQueue> q = makeQueue(name, flights.size());
    RunResult r = runSimulation(*q, s);

    size_t late = 0, cancelled = 0;
    for (const Entry &e : s.changes)
        if (e.arrival > flights[e.id].schedArrival)
            late++;
    for (int id : s.removals)
        if (flights[id].cancelled)
            cancelled++;

    std::cout << "Atlanta (ATL) arrivals: the first " << flights.size() << " flights in " << DATA << "\n"
              << "  flight plans added      " << std::setw(7) << s.plans.size() << "\n"
              << "  arrival times changed   " << std::setw(7) << s.changes.size() << "  (" << late
              << " late, " << s.changes.size() - late << " early)\n"
              << "  cancelled or diverted   " << std::setw(7) << s.removals.size() << "  (" << cancelled
              << " cancelled, " << s.removals.size() - cancelled << " diverted)\n"
              << "  landed                  " << std::setw(7) << r.landingOrder.size()
              << (r.landingOrder == s.expectedOrder ? "  in the real arrival order\n"
                                                    : "  NOT in the real arrival order\n")
              << "\nFirst landings, using " << q->name() << ":\n"
              << "    #  departed    flight   from  scheduled  landed  change\n";

    for (size_t i = 0; i < r.landingOrder.size() && i < 15; i++)
    {
        const Flight &f = flights[r.landingOrder[i]];
        std::string change = f.arrDelay == 0 ? "on time"
                           : f.arrDelay > 0  ? std::to_string(f.arrDelay) + " min late"
                                             : std::to_string(-f.arrDelay) + " min early";
        std::cout << std::setw(5) << i + 1 << "  " << f.date << "  " << std::left << std::setw(3) << f.airline
                  << std::right << std::setw(5) << f.number << "   " << f.origin << "   " << std::setw(8)
                  << clockTime(f.schedArrival) << "  " << std::setw(6) << clockTime(f.actualArrival()) << "  "
                  << change << "\n";
    }
    return 0;
}

static int check(int n)
{
    std::vector<Flight> flights = loadFlights(DATA, n);
    Schedule s = makeSchedule(flights);
    std::cout << "Landing order check on " << flights.size() << " flights\n";

    int failures = 0;
    for (const std::string &name : structureNames())
    {
        std::string missing;
        if (!isWritten(name, missing))
        {
            std::cout << "  SKIP  " << name << ": " << missing << "\n";
            continue;
        }
        try
        {
            std::unique_ptr<ArrivalQueue> q = makeQueue(name, flights.size());
            RunResult r = runSimulation(*q, s);
            if (r.landingOrder == s.expectedOrder)
                std::cout << "  OK    " << name << ": " << r.landingOrder.size()
                          << " flights landed in the real arrival order\n";
            else
            {
                failures++;
                std::cout << "  FAIL  " << name << ": " << firstDifference(r.landingOrder, s.expectedOrder) << "\n";
            }
        }
        catch (const std::exception &e)
        {
            failures++;
            std::cout << "  FAIL  " << name << ": " << e.what() << "\n";
        }
    }
    return failures == 0 ? 0 : 1;
}

static int bench(int runs)
{
    std::vector<Flight> all = loadFlights(DATA);  // loading the data is never timed
    const int sizes[] = {1000, 10000, 100000};

    std::ofstream csv(RESULTS);
    if (!csv)
        std::cout << "warning: can't write " << RESULTS << "; printing averages only\n";
    csv << "structure,flights,run,add_ms,delay_ms,remove_ms,land_ms,total_ms\n";

    std::cout << "Benchmark: " << runs << " runs per size. Loading the data is not timed.\n"
              << "Average milliseconds per run:\n\n"
              << "structure       flights        add      delay     remove       land      total\n"
              << std::fixed << std::setprecision(3);

    int failures = 0;
    for (const std::string &name : structureNames())
    {
        std::string missing;
        if (!isWritten(name, missing))
        {
            std::cout << std::left << std::setw(14) << name << std::right << "  skipped: " << missing << "\n";
            continue;
        }
        for (int n : sizes)
        {
            if (n > (int)all.size())
                continue;
            std::vector<Flight> flights(all.begin(), all.begin() + n);
            Schedule s = makeSchedule(flights);
            double add = 0, delay = 0, remove = 0, land = 0;
            bool rightOrder = true;
            try
            {
                for (int run = 1; run <= runs; run++)
                {
                    std::unique_ptr<ArrivalQueue> q = makeQueue(name, n);
                    RunResult r = runSimulation(*q, s);
                    rightOrder = rightOrder && r.landingOrder == s.expectedOrder;
                    csv << name << ',' << n << ',' << run << ',' << r.addMs << ',' << r.delayMs << ','
                        << r.removeMs << ',' << r.landMs << ',' << r.totalMs() << '\n';
                    add += r.addMs;
                    delay += r.delayMs;
                    remove += r.removeMs;
                    land += r.landMs;
                }
            }
            catch (const std::exception &e)
            {
                failures++;
                std::cout << std::left << std::setw(14) << name << std::right << "  FAILED: " << e.what() << "\n";
                break;
            }
            std::cout << std::left << std::setw(14) << name << std::right << std::setw(8) << n << std::setw(11)
                      << add / runs << std::setw(11) << delay / runs << std::setw(11) << remove / runs
                      << std::setw(11) << land / runs << std::setw(11) << (add + delay + remove + land) / runs
                      << (rightOrder ? "" : "  WRONG LANDING ORDER") << std::endl;
            if (!rightOrder)
                failures++;
        }
    }
    std::cout << "\nEvery run is in " << RESULTS << ".\n";
    return failures == 0 ? 0 : 1;
}

int main(int argc, char *argv[])
{
    std::string command = argc > 1 ? argv[1] : "";
    try
    {
        if (command == "demo")
            return demo(argc > 2 ? std::atoi(argv[2]) : 1000, argc > 3 ? argv[3] : "sorted-vector");
        if (command == "check")
            return check(argc > 2 ? std::atoi(argv[2]) : 0);
        if (command == "bench")
            return bench(argc > 2 ? std::atoi(argv[2]) : 5);
    }
    catch (const std::exception &e)
    {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    usage();
    return command.empty() ? 0 : 1;
}
