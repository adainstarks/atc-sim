// A.I. Disclaimer: TODO (owner) - Gemini was used to get help with codes, debugging and comment lines.
/*
 PROGRAMMER:          Pau Mang
 Program Description: Structure C, the indexed heap (see IndexedHeapQueue.h).
*/
#include "IndexedHeapQueue.h"
#include <algorithm>

IndexedHeapQueue::IndexedHeapQueue(int maxFlights)
{
    // reserve some space for performance
    heap.reserve(maxFlights);
}

void IndexedHeapQueue::addFlight(int id, int arrival)
{
    // duplicate flight id check
    auto it = pos.find(id);
    if (it != pos.end()) {
        delayFlight(id, arrival);
        return;
    }

    heap.push_back(Entry{arrival, id});
    int index = heap.size() - 1;
    pos[id] = index;
    siftUp(index);
}

void IndexedHeapQueue::delayFlight(int id, int newArrival)
{
    auto it = pos.find(id);
    if (it == sim_end_check_or_not_found(it, pos)) // standard iterator check
        return;
    if (it == pos.end()) 
        return;
    int i = it->second;
    
    // Update arrival while preserving the id
    int flightId = heap[i].id;
    heap[i] = Entry{newArrival, flightId};

    int parentIndex = (i - 1) / 2;
    if (i > 0 && heap[i] < heap[parentIndex]) {
        siftUp(i);
    } else {
        siftDown(i);
    }
}

void IndexedHeapQueue::removeFlight(int id)
{
    auto it = pos.find(id);
    if (it == pos.end()) return;
    int i = it->second;
    pos.erase(id);
 
    int lastIndex = heap.size() - 1;
    if (i == lastIndex) {
        heap.pop_back();
        return;
    }
    heap[i] = heap[lastIndex]; // shift last into recently removed spot
    pos[heap[i].id] = i;
    heap.pop_back();

    int parentIndex = (i - 1) / 2;
    if (i > 0 && heap[i] < heap[parentIndex]) {
        siftUp(i);
    } else {
        siftDown(i);
    }
}

bool IndexedHeapQueue::landNext(int &id, int &arrival)
{
    if (heap.empty()) {
         return false;
    }
    id = heap[0].id;
    arrival = heap[0].arrival;
    pos.erase(id);
    if (heap.size() == 1) {
        heap.pop_back();
        return true;
    }
    heap[0] = heap.back();
    heap.pop_back();
    pos[heap[0].id] = 0;
    siftDown(0);
    return true;
}

int IndexedHeapQueue::size() const
{
    return heap.size();
}

// helpers
void IndexedHeapQueue::siftUp(int i)
{
    while (i > 0) {
        int parentIndex = (i - 1) / 2;
        // Compare whole entries using Entry's overloaded operator<
        if (heap[i] < heap[parentIndex]) {
            std::swap(heap[i], heap[parentIndex]);
            pos[heap[i].id] = i;
            pos[heap[parentIndex].id] = parentIndex;
            i = parentIndex;
        } else {
            break;
        }
    }
}

void IndexedHeapQueue::siftDown(int i)
{
    int n = heap.size();
    while (2 * i + 1 < n) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smallest = i;
        
        // Compare whole entries using Entry's overloaded operator<
        if (heap[left] < heap[smallest]) {
           smallest = left;
        }
        if (right < n && heap[right] < heap[smallest]) {
           smallest = right;
        }
        
        if (smallest != i) {
            std::swap(heap[i], heap[smallest]);
            pos[heap[i].id] = i;
            pos[heap[smallest].id] = smallest;
            i = smallest;
        } else {
            break;
        }
    }
}
