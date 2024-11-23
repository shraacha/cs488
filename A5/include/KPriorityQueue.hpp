#pragma once

#include <functional>
#include <vector>
#include <algorithm>

// - makes a max priority queue,
// - comp should be '<'
template <typename T> class KPriorityQueue
{
  public:
    KPriorityQueue(size_t k, std::function<bool(const T &, const T &)> comp)
        : m_size{k}, m_comp{comp}
    {}

    bool isFull() { return m_heap.size() == m_size; }

    T getTop() { return m_heap.front(); }

    void push(const T & elem)
    {
        if (!isFull())
        {
            m_heap.push_back(elem);
            std::push_heap(m_heap.begin(), m_heap.end(), m_comp);
        }
        else
        {
            // only add to heap if the provided element is < top
            if (m_comp(elem, getTop()))
            {
                m_heap.push_back(elem);
                std::push_heap(m_heap.begin(), m_heap.end(), m_comp);
                std::pop_heap(m_heap.begin(), m_heap.end(), m_comp);
                m_heap.pop_back();
            }
        }
    }

    std::vector<T> & getQueue() { return m_heap; }

  private:
    std::vector<T> m_heap;
    size_t m_size;
    std::function<bool(const T &, const T &)> m_comp;
};
