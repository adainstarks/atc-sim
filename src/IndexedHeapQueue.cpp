// A.I. Disclaimer: TODO (owner) - Gemini was used to get help with codes, debugging and comment lines.
/*
 PROGRAMMER:          Pau Mang
 Program Description: Structure C, the indexed heap (see IndexedHeapQueue.h).
*/
#include "IndexedHeapQueue.h"
#include <algorithm>

IndexedHeapQueue::IndexedHeapQueue(int maxFlights)
{
    heap.reserve(maxFlights);
}

void IndexedHeapQueue::addFlight(int id, int arrival)
{
    auto it = pos.find(id);
    if (it != pos.end()) {
        delayFlight(id, arrival);
        return;
    }

    heap.push_back(Entry{ arrival, id });
    int index = static_cast<int>(heap.size()) - 1;
    pos[id] = index;
    siftUp(index);
}

void IndexedHeapQueue::delayFlight(int id, int newArrival)
{
    auto it = pos.find(id);
    if (it == pos.end()) {
        return;
    }

    int i = it->second;
    heap[i].arrival = newArrival;

    int parentIndex = (i - 1) / 2;
    if (i > 0 && heap[i] < heap[parentIndex]) {
        siftUp(i);
    }
    else {
        siftDown(i);
    }
}

void IndexedHeapQueue::removeFlight(int id)
{
    auto it = pos.find(id);
    if (it == pos.end()) {
        return;
    }

    int i = it->second;
    pos.erase(id);

    int lastIndex = static_cast<int>(heap.size()) - 1;
    if (i == lastIndex) {
        heap.pop_back();
        return;
    }

    heap[i] = heap[lastIndex];
    pos[heap[i].id] = i;
    heap.pop_back();

    int parentIndex = (i - 1) / 2;
    if (i > 0 && heap[i] < heap[parentIndex]) {
        siftUp(i);
    }
    else {
        siftDown(i);
    }
}

bool IndexedHeapQueue::landNext(int& id, int& arrival)
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
    return static_cast<int>(heap.size());
}

void IndexedHeapQueue::siftUp(int i)
{
    while (i > 0) {
        int parentIndex = (i - 1) / 2;
        if (heap[i] < heap[parentIndex]) {
            std::swap(heap[i], heap[parentIndex]);
            pos[heap[i].id] = i;
            pos[heap[parentIndex].id] = parentIndex;
            i = parentIndex;
        }
        else {
            break;
        }
    }
}

void IndexedHeapQueue::siftDown(int i)
{
    int n = static_cast<int>(heap.size());
    while (2 * i + 1 < n) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smallest = i;

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
        }
        else {
            break;
        }
    }
}
