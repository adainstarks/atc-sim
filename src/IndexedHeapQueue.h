// A.I. Disclaimer: completed with help from OpenAI ChatGPT, an AI assistant. The semester
// project allows AI with disclosure; the team checks this code with the tests in tests/.
/*
 PROGRAMMER:          Pau Mang
 Program Description: Structure C. The arrival list as a binary min-heap plus a position array
                      pos_[id], so any flight can be found and moved in O(log n) with no
                      stale copies.
*/
#ifndef INDEXEDHEAPQUEUE_H
#define INDEXEDHEAPQUEUE_H
#include "ArrivalQueue.h"
#include <vector>
class IndexedHeapQueue : public ArrivalQueue {
public:
 explicit IndexedHeapQueue(int maxFlights);
 std::string name() const override { return "indexed-heap"; }
 void addFlight(int id,int arrival) override;
 void delayFlight(int id,int newArrival) override;
 void removeFlight(int id) override;
 bool landNext(int &id,int &arrival) override;
 int size() const override { return static_cast<int>(heap_.size()); }
private:
 std::vector<Entry> heap_;
 std::vector<int> pos_;
 void validate(int id) const;
 void swapAt(int i,int j);
 void siftUp(int i);
 void siftDown(int i);
 void eraseAt(int i);
};
#endif
