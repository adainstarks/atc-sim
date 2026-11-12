// A.I. Disclaimer: completed with help from OpenAI ChatGPT, an AI assistant. The semester
// project allows AI with disclosure; the team checks this code with the tests in tests/.
/*
 PROGRAMMER:          Kyle Penton
 Program Description: Structure B. The arrival list as a binary heap with lazy deletion: a changed
                      flight gets a new heap node with a new generation number, and the old
                      node is skipped when it reaches the top.
*/
#ifndef LAZYHEAPQUEUE_H
#define LAZYHEAPQUEUE_H
#include "ArrivalQueue.h"
#include <queue>
#include <vector>
#include <functional>
#include <cstdint>
class LazyHeapQueue : public ArrivalQueue {
public:
 explicit LazyHeapQueue(int maxFlights);
 std::string name() const override { return "lazy-heap"; }
 void addFlight(int id,int arrival) override;
 void delayFlight(int id,int newArrival) override;
 void removeFlight(int id) override;
 bool landNext(int &id,int &arrival) override;
 int size() const override { return count_; }
private:
 struct Node { Entry entry; uint64_t generation; };
 struct Compare { bool operator()(const Node &a,const Node &b) const { return b.entry < a.entry; } };
 std::priority_queue<Node,std::vector<Node>,Compare> heap_;
 std::vector<int> arrivals_;
 std::vector<uint64_t> generations_;
 std::vector<bool> active_;
 int count_=0;
 void validate(int id) const;
};
#endif
