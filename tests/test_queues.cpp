// A.I. Disclaimer: written with help from Claude (Anthropic), an AI assistant. The semester
// project allows AI with disclosure; the team checks this code by running it.
/*
 PROGRAMMER:          William Starks
 Program Description: Unit tests for every arrival-list structure. Each test runs on each
                      structure; anything that still throws NotWrittenYet is skipped.
   Output: PASS / FAIL / SKIP per test. Exit code 1 if anything failed.
*/
#include "Simulation.h"

#include <iostream>
#include <iterator>
#include <random>
#include <set>

// A test fails by throwing this.
class TestFailed : public std::runtime_error
{
public:
    explicit TestFailed(const std::string &what) : std::runtime_error(what) {}
};

static void expect(bool ok, const std::string &what)
{
    if (!ok)
        throw TestFailed(what);
}

static std::string show(const std::vector<int> &ids)
{
    std::string s = "[";
    for (size_t i = 0; i < ids.size(); i++)
        s += (i ? ", " : "") + std::to_string(ids[i]);
    return s + "]";
}

// Lands every flight left in q and checks they come out in the order `want`.
static void expectLandings(ArrivalQueue &q, const std::vector<int> &want)
{
    std::vector<int> got;
    int id, arrival;
    while (got.size() <= want.size() + 5 && q.landNext(id, arrival))
        got.push_back(id);
    expect(got == want, "landed " + show(got) + ", expected " + show(want));
    expect(q.size() == 0, "size should be 0 after everything landed, got " + std::to_string(q.size()));
}

static void emptyList(const std::string &name)
{
    std::unique_ptr<ArrivalQueue> q = makeQueue(name, 10);
    int id, arrival;
    expect(q->size() == 0, "a new list should have size 0");
    expect(!q->landNext(id, arrival), "landNext on an empty list should return false");
}

static void earliestFirst(const std::string &name)
{
    std::unique_ptr<ArrivalQueue> q = makeQueue(name, 10);
    q->addFlight(0, 50);
    q->addFlight(1, 10);
    q->addFlight(2, 30);
    expect(q->size() == 3, "size should be 3 after adding 3 flights");
    int id = -1, arrival = -1;
    expect(q->landNext(id, arrival), "landNext should return true when flights are waiting");
    expect(id == 1 && arrival == 10, "first landing should be flight 1 at minute 10, got flight " +
                                         std::to_string(id) + " at " + std::to_string(arrival));
    expectLandings(*q, {2, 0});
}

static void sameMinuteById(const std::string &name)
{
    std::unique_ptr<ArrivalQueue> q = makeQueue(name, 10);
    q->addFlight(5, 20);
    q->addFlight(2, 20);
    q->addFlight(9, 20);
    q->addFlight(7, 25);
    expectLandings(*q, {2, 5, 9, 7});
}

static void delayedFlightMovesBack(const std::string &name)
{
    std::unique_ptr<ArrivalQueue> q = makeQueue(name, 10);
    q->addFlight(0, 10);
    q->addFlight(1, 20);
    q->addFlight(2, 30);
    q->delayFlight(0, 25);
    expect(q->size() == 3, "a delay shouldn't change the size");
    int id = -1, arrival = -1;
    q->landNext(id, arrival);
    expect(id == 1 && arrival == 20, "first landing should be flight 1 at minute 20");
    q->landNext(id, arrival);
    expect(id == 0 && arrival == 25, "second landing should be flight 0 at its new time, minute 25, got flight " +
                                         std::to_string(id) + " at " + std::to_string(arrival));
    expectLandings(*q, {2});
}

static void earlyFlightMovesUp(const std::string &name)
{
    std::unique_ptr<ArrivalQueue> q = makeQueue(name, 10);
    q->addFlight(0, 10);
    q->addFlight(1, 20);
    q->addFlight(2, 30);
    q->delayFlight(2, 5);
    expectLandings(*q, {2, 0, 1});
}

static void removedFlightNeverLands(const std::string &name)
{
    std::unique_ptr<ArrivalQueue> q = makeQueue(name, 10);
    q->addFlight(0, 10);
    q->addFlight(1, 20);
    q->addFlight(2, 30);
    q->removeFlight(1);
    expect(q->size() == 2, "size should be 2 after removing 1 of 3 flights");
    expectLandings(*q, {0, 2});
}

static void delayedThenMovedBack(const std::string &name)
{
    std::unique_ptr<ArrivalQueue> q = makeQueue(name, 10);
    q->addFlight(0, 10);
    q->addFlight(1, 20);
    q->delayFlight(0, 30);
    q->delayFlight(0, 10);  // back to its original time: must still land exactly once
    expect(q->size() == 2, "size should still be 2");
    expectLandings(*q, {0, 1});
}

static void removedAfterDelay(const std::string &name)
{
    std::unique_ptr<ArrivalQueue> q = makeQueue(name, 10);
    q->addFlight(0, 10);
    q->addFlight(1, 20);
    q->delayFlight(0, 40);
    q->removeFlight(0);
    expect(q->size() == 1, "size should be 1");
    expectLandings(*q, {1});
}

static void sizeCountsWaitingFlights(const std::string &name)
{
    std::unique_ptr<ArrivalQueue> q = makeQueue(name, 10);
    q->addFlight(0, 10);
    q->addFlight(1, 20);
    q->addFlight(2, 30);
    q->delayFlight(1, 35);
    q->delayFlight(1, 15);
    expect(q->size() == 3, "size should be 3 after two delays, got " + std::to_string(q->size()));
    int id, arrival;
    q->landNext(id, arrival);
    expect(q->size() == 2, "size should be 2 after one landing, got " + std::to_string(q->size()));
    q->removeFlight(2);
    expect(q->size() == 1, "size should be 1 after a removal, got " + std::to_string(q->size()));
    expectLandings(*q, {1});
}

// Thousands of random operations, checked step by step against a simple std::set version.
static void matchesReference(const std::string &name)
{
    const int maxFlights = 2000;
    std::unique_ptr<ArrivalQueue> q = makeQueue(name, maxFlights);
    std::set<Entry> reference;
    std::vector<int> arrivalOf(maxFlights, -1);
    int nextId = 0;
    std::mt19937 rng(42);

    for (int step = 0; step < 12000; step++)
    {
        int roll = rng() % 10;
        std::string what;
        if (roll < 4 && nextId < maxFlights)
        {
            int id = nextId++, arrival = rng() % 500;  // a small range, so many flights share a minute
            what = "addFlight(" + std::to_string(id) + ", " + std::to_string(arrival) + ")";
            q->addFlight(id, arrival);
            reference.insert({arrival, id});
            arrivalOf[id] = arrival;
        }
        else if (roll < 7 && !reference.empty())
        {
            std::set<Entry>::iterator it = reference.begin();
            std::advance(it, rng() % reference.size());
            int id = it->id;
            bool remove = roll == 6;
            if (remove)
            {
                what = "removeFlight(" + std::to_string(id) + ")";
                q->removeFlight(id);
                reference.erase(it);
            }
            else
            {
                int arrival = rng() % 500;
                what = "delayFlight(" + std::to_string(id) + ", " + std::to_string(arrival) + ")";
                q->delayFlight(id, arrival);
                reference.erase(it);
                reference.insert({arrival, id});
                arrivalOf[id] = arrival;
            }
        }
        else
        {
            int id = -1, arrival = -1;
            bool landed = q->landNext(id, arrival);
            what = "landNext()";
            expect(landed == !reference.empty(), "step " + std::to_string(step) + ": landNext returned " +
                                                     (landed ? "true" : "false") + " with " +
                                                     std::to_string(reference.size()) + " flights waiting");
            if (landed)
            {
                Entry want = *reference.begin();
                reference.erase(reference.begin());
                expect(id == want.id && arrival == want.arrival,
                       "step " + std::to_string(step) + ": landed flight " + std::to_string(id) + " at " +
                           std::to_string(arrival) + ", expected flight " + std::to_string(want.id) + " at " +
                           std::to_string(want.arrival));
            }
        }
        expect(q->size() == (int)reference.size(), "step " + std::to_string(step) + " (" + what + "): size is " +
                                                       std::to_string(q->size()) + ", expected " +
                                                       std::to_string(reference.size()));
    }
}

struct Test
{
    const char *title;
    void (*run)(const std::string &name);
};

int main()
{
    const Test tests[] = {
        {"empty list", emptyList},
        {"lands the earliest flight first", earliestFirst},
        {"flights due the same minute land in id order", sameMinuteById},
        {"a delayed flight moves back in line", delayedFlightMovesBack},
        {"an early flight moves up in line", earlyFlightMovesUp},
        {"a removed flight never lands", removedFlightNeverLands},
        {"delayed, then moved back to its old time", delayedThenMovedBack},
        {"removed after a delay", removedAfterDelay},
        {"size counts waiting flights only", sizeCountsWaitingFlights},
        {"12,000 random operations match a reference", matchesReference},
    };

    int failed = 0;
    for (const std::string &name : structureNames())
    {
        std::vector<std::string> lines;
        int passed = 0, skipped = 0;
        std::string firstMissing;
        for (const Test &t : tests)
        {
            try
            {
                t.run(name);
                lines.push_back(std::string("  PASS  ") + t.title);
                passed++;
            }
            catch (const NotWrittenYet &e)
            {
                lines.push_back(std::string("  SKIP  ") + t.title + ": " + e.what());
                if (firstMissing.empty())
                    firstMissing = e.what();
                skipped++;
            }
            catch (const std::exception &e)
            {
                lines.push_back(std::string("  FAIL  ") + t.title + ": " + e.what());
                failed++;
            }
        }
        std::cout << name << "\n";
        if (skipped == (int)lines.size())
            std::cout << "  SKIP  all tests: " << firstMissing << "\n";
        else
            for (const std::string &line : lines)
                std::cout << line << "\n";
        std::cout << "  " << passed << " passed, " << lines.size() - passed - skipped << " failed, " << skipped
                  << " skipped\n\n";
    }
    std::cout << (failed == 0 ? "No failures.\n" : "Some tests FAILED.\n");
    return failed == 0 ? 0 : 1;
}
