// A.I. Disclaimer: completed with help from OpenAI ChatGPT, an AI assistant. The semester
// project allows AI with disclosure; the team checks this code with the tests in tests/.
/*
 PROGRAMMER:          Pau Mang
 Program Description: Structure C, the indexed heap (see IndexedHeapQueue.h).
*/
#include "IndexedHeapQueue.h"
#include <stdexcept>
#include <utility>
IndexedHeapQueue::IndexedHeapQueue(int maxFlights):pos_(maxFlights,-1) { heap_.reserve(maxFlights); }
void IndexedHeapQueue::validate(int id) const { if(id<0 || id>=static_cast<int>(pos_.size())) throw std::out_of_range("flight id"); }
void IndexedHeapQueue::swapAt(int i,int j) { std::swap(heap_[i],heap_[j]); pos_[heap_[i].id]=i; pos_[heap_[j].id]=j; }
void IndexedHeapQueue::siftUp(int i) { while(i>0) { int p=(i-1)/2; if(!(heap_[i]<heap_[p])) break; swapAt(i,p); i=p; } }
void IndexedHeapQueue::siftDown(int i) { int n=heap_.size(); while(true) { int l=2*i+1,r=l+1,b=i; if(l<n && heap_[l]<heap_[b]) b=l; if(r<n && heap_[r]<heap_[b]) b=r; if(b==i) break; swapAt(i,b); i=b; } }
void IndexedHeapQueue::addFlight(int id,int arrival) { validate(id); if(pos_[id]!=-1) throw std::logic_error("duplicate flight id"); pos_[id]=heap_.size(); heap_.push_back({arrival,id}); siftUp(pos_[id]); }
void IndexedHeapQueue::delayFlight(int id,int arrival) { validate(id); int i=pos_[id]; if(i==-1) throw std::logic_error("flight not active"); Entry old=heap_[i]; heap_[i].arrival=arrival; if(heap_[i]<old) siftUp(i); else if(old<heap_[i]) siftDown(i); }
void IndexedHeapQueue::eraseAt(int i) { int id=heap_[i].id; int last=heap_.size()-1; pos_[id]=-1; if(i==last) {heap_.pop_back(); return;} heap_[i]=heap_[last]; heap_.pop_back(); pos_[heap_[i].id]=i; if(i>0 && heap_[i]<heap_[(i-1)/2]) siftUp(i); else siftDown(i); }
void IndexedHeapQueue::removeFlight(int id) { validate(id); if(pos_[id]==-1) throw std::logic_error("flight not active"); eraseAt(pos_[id]); }
bool IndexedHeapQueue::landNext(int &id,int &arrival) { if(heap_.empty()) return false; id=heap_[0].id; arrival=heap_[0].arrival; eraseAt(0); return true; }
