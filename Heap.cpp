#include <vector>
#include <functional>
#include <stdexcept>
#include <utility>

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
