// A.I. Disclaimer: completed with help from OpenAI ChatGPT, an AI assistant. The semester
// project allows AI with disclosure; the team checks this code with the tests in tests/.
/*
 PROGRAMMER:          Kyle Penton
 Program Description: Structure B, the heap with lazy deletion (see LazyHeapQueue.h).
*/
#include "LazyHeapQueue.h"
#include <stdexcept>
LazyHeapQueue::LazyHeapQueue(int maxFlights):arrivals_(maxFlights),generations_(maxFlights,0),active_(maxFlights,false) {}
void LazyHeapQueue::validate(int id) const { if(id<0 || id>=static_cast<int>(active_.size())) throw std::out_of_range("flight id"); }
void LazyHeapQueue::addFlight(int id,int arrival) {
 validate(id); if(active_[id]) throw std::logic_error("duplicate flight id");
 active_[id]=true; arrivals_[id]=arrival; ++generations_[id]; heap_.push({{arrival,id},generations_[id]}); ++count_;
}
void LazyHeapQueue::delayFlight(int id,int arrival) {
 validate(id); if(!active_[id]) throw std::logic_error("flight not active");
 arrivals_[id]=arrival; ++generations_[id]; heap_.push({{arrival,id},generations_[id]});
}
void LazyHeapQueue::removeFlight(int id) {
 validate(id); if(!active_[id]) throw std::logic_error("flight not active");
 active_[id]=false; ++generations_[id]; --count_;
}
bool LazyHeapQueue::landNext(int &id,int &arrival) {
 while(!heap_.empty()) {
  Node n=heap_.top(); heap_.pop(); int k=n.entry.id;
  if(!active_[k] || generations_[k]!=n.generation) continue;
  id=k; arrival=n.entry.arrival; active_[k]=false; ++generations_[k]; --count_; return true;
 }
 return false;
}
