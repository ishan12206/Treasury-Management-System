#include <vector>
#include <functional>
#include <stdexcept>
#include <utility>
#include <algorithm>
#include <iostream>
#include <queue>
#include <tuple>

// Forward declarations
class Treasure;
class CrewMate;

// Comparison function for crew
bool comp_crew(const CrewMate& c1, const CrewMate& c2);

// Comparison function for treasures
bool comp_treasures(const std::pair<int, Treasure*>& t1, const std::pair<int, Treasure*>& t2);

// Treasure class
class Treasure {
public:
    int id;
    int size;
    int arrival_time;
    int completion_time;

    Treasure(int i, int s, int a) : id(i), size(s), arrival_time(a), completion_time(-1) {}
};

// CrewMate class
class CrewMate {
public:
    int load;
    std::vector<Treasure*> treasures;

    CrewMate() : load(0) {}
};

bool comp_crew(const CrewMate& c1, const CrewMate& c2) {
    return c1.load < c2.load;
}

bool comp_treasures(const std::pair<int, Treasure*>& t1, const std::pair<int, Treasure*>& t2) {
    int sum1 = t1.first + t1.second->arrival_time;
    int sum2 = t2.first + t2.second->arrival_time;
    if (sum1 != sum2) return sum1 < sum2;
    return t1.second->id < t2.second->id;
}

// Template Heap class

template <typename T>
class Heap {
private:
    std::vector<T> heap;
    std::function<bool(const T&, const T&)> comp_func;

    int lc(int i) const { return 2 * i + 1; }
    int rc(int i) const { return 2 * i + 2; }
    int parent(int i) const { return (i - 1) / 2; }

    bool lc_exists(int i) const { return lc(i) < heap.size(); }
    bool rc_exists(int i) const { return rc(i) < heap.size(); }

    void swap_data(int i, int j) {
        std::swap(heap[i], heap[j]);
    }

    void heapify() {
        for (int i = parent(heap.size() - 1); i >= 0; --i) {
            downheap(i);
        }
    }

    void downheap(int index) {
        if (lc_exists(index)) {
            int l = lc(index);
            int smallest = l;
            if (rc_exists(index)) {
                int r = rc(index);
                if (!comp_func(heap[l], heap[r])) {
                    smallest = r;
                }
            }
            if (comp_func(heap[smallest], heap[index])) {
                swap_data(smallest, index);
                downheap(smallest);
            }
        }
    }

    void upheap(int index) {
        int p = parent(index);
        if (index > 0 && comp_func(heap[index], heap[p])) {
            swap_data(index, p);
            upheap(p);
        }
    }

public:
    Heap(std::function<bool(const T&, const T&)> comparison_function, const std::vector<T>& init_array)
        : heap(init_array), comp_func(comparison_function) {
        heapify();
    }

    void insert(const T& value) {
        heap.push_back(value);
        upheap(heap.size() - 1);
    }

    T extract() {
        if (heap.empty()) {
            throw std::runtime_error("Cannot extract from an empty heap");
        }
        swap_data(0, heap.size() - 1);
        T top_value = heap.back();
        heap.pop_back();
        downheap(0);
        return top_value;
    }

    T top() const {
        if (heap.empty()) {
            throw std::runtime_error("Heap is empty");
        }
        return heap[0];
    }

    bool is_empty() const {
        return heap.empty();
    }

    int size() const {
        return heap.size();
    }
};

// StrawHatTreasury class
class StrawHatTreasury {
private:
    std::vector<CrewMate> crew;
    Heap<CrewMate> crewheap;

public:
    StrawHatTreasury(int m)
        : crew(m), crewheap(comp_crew, crew) {}

    void add_treasure(Treasure* treasure) {
        CrewMate crew_mate = crewheap.extract();
        int new_load = crew_mate.treasures.empty()
            ? std::max(treasure->arrival_time, crew_mate.load) + treasure->size
            : std::max(treasure->arrival_time - crew_mate.treasures.back()->arrival_time, crew_mate.load) + treasure->size;

        crew_mate.load = new_load;
        crew_mate.treasures.push_back(treasure);
        crewheap.insert(crew_mate);
    }

    std::vector<Treasure*> get_completion_time() {
        std::vector<Treasure*> processed;

        for (CrewMate& cm : crew) {
            int time = 0;
            if (!cm.treasures.empty()) {
                Heap<std::pair<int, Treasure*>> treasure_heap(comp_treasures, {});

                for (Treasure* t : cm.treasures) {
                    while (!treasure_heap.is_empty()) {
                        auto [size_left, curr_treasure] = treasure_heap.extract();
                        int time_until_arrival = t->arrival_time - time;

                        if (size_left > time_until_arrival) {
                            treasure_heap.insert({size_left - time_until_arrival, curr_treasure});
                            treasure_heap.insert({t->size, t});
                            time = t->arrival_time;
                            goto next_treasure;
                        } else {
                            time += size_left;
                            curr_treasure->completion_time = time;
                            processed.push_back(curr_treasure);
                        }
                    }
                    treasure_heap.insert({t->size, t});
                    time = t->arrival_time;
                next_treasure:;
                }

                while (!treasure_heap.is_empty()) {
                    auto [size_left, t] = treasure_heap.extract();
                    time += size_left;
                    t->completion_time = time;
                    processed.push_back(t);
                }
            }
        }

        std::sort(processed.begin(), processed.end(), [](Treasure* a, Treasure* b) {
            return a->id < b->id;
        });

        return processed;
    }
};
