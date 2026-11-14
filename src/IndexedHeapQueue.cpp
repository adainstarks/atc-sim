// A.I. Disclaimer: TODO (owner) - Gemini was used to get help with codes, debugging and comment lines.
/*
 PROGRAMMER:          Pau Mang
 Program Description: Structure C, the indexed heap (see IndexedHeapQueue.h).
*/
#include "IndexedHeapQueue.h"
#include <algorithm>

// Each function below throws NotWrittenYet until it is written. Replace the throw with
// the real code; the tests skip whatever still throws.

#include "IndexedHeapQueue.h"
#include <algorithm>

IndexedHeapQueue::IndexedHeapQueue(int maxFlights)
{
    // reserve some space for performance
    heap.reserve(maxFlights);
}

void IndexedHeapQueue::addFlight(int id, int arrival)
{
    // Fixed typo: pso -> pos
    auto it = pos.find(id);
    if (it != pos.end()) {
        delayFlight(id, arrival);
        return;
    }

    // Fixed pushBack -> push_back
    heap.push_back(Entry(id, arrival));
    int index = heap.size() - 1;
    pos[id] = index;
    siftUp(index);
}

void IndexedHeapQueue::delayFlight(int id, int newArrival)
{
    auto it = pos.find(id);
    // Fixed syntax error (missing closing parenthesis and return)
    if (it == pos.end()) {
        return;
    }
    
    int i = it->second;
    int oldArrival = heap[i].arrival;
    heap[i].arrival = newArrival;
    
    if (newArrival < oldArrival) {
        siftUp(i);
    } else if (newArrival > oldArrival) {
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
        heap.pop_back(); // Fixed popBack -> pop_back
        return;
    }
    
    heap[i] = heap[lastIndex]; // shift last into recently removed spot
    pos[heap[i].id] = i;
    heap.pop_back(); // Fixed popBack -> pop_back

    int parentIndex = (i - 1) / 2; // Added missing semicolon
    if (i > 0 && heap[i].arrival < heap[parentIndex].arrival) {
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
        heap.pop_back(); // Fixed popBack -> pop_back
        return true;
    }
    
    heap[0] = heap.back();
    heap.pop_back(); // Fixed popBack -> pop_back
    pos[heap[0].id] = 0;
    siftDown(0);
    return true;
}

int IndexedHeapQueue::size() const
{
    return heap.size();
}

// Helpers
void IndexedHeapQueue::siftUp(int i)
{
    while (i > 0) {
        int parentIndex = (i - 1) / 2;
        if (heap[i].arrival < heap[parentIndex].arrival) {
            std::swap(heap[i], heap[parentIndex]); // Added missing closing parenthesis
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

        // Fixed siftDown logic to properly check left and right children
        if (left < n && heap[left].arrival < heap[smallest].arrival) {
            smallest = left;
        }
        if (right < n && heap[right].arrival < heap[smallest].arrival) {
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
