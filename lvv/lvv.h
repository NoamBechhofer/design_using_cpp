#ifndef LVV_H
#define LVV_H

#include <cassert>
#include <chrono>
#include <future>
#include <list>
#include <random>
#include <unordered_set>
#include <vector>

/**
 * @brief A random number generator
 * @details be aware that this global generator is reseeded at points
 */
std::mt19937 gen{std::random_device{}()};

constexpr int DEFAULT_RUNS_PER_TEST = 3;

namespace utils {

    /**
     * @brief generate a uniformly distributed random integer between min and
     * max (inclusive)
     *
     * @param min minimum value (inclusive)
     * @param max maximum value (inclusive)
     * @return int a random integer between min and max (inclusive)
     */
    int random_int(int min, int max)
    {
        auto dist = std::uniform_int_distribution<>(min, max);
        return dist(gen);
    }

    /**
     * @brief generate a set of n uniformly distributed random integers between
     * min and max (inclusive)
     *
     * @param n number of integers to generate
     * @param min minimum value (inclusive)
     * @param max maximum value (inclusive)
     * @param gen random number generator
     * @return set<int> a set of n uniformly distributed random integers between
     * min and max (inclusive)
     */
    std::unordered_set<int> generate_n_random_ints(size_t n, int min, int max)
    {
        std::unordered_set<int> s;
        while(s.size() < n) { s.insert(random_int(min, max)); }
        return s;
    }

    /**
     * @brief insert n into v in numerical order
     *
     * @param v vector to insert into
     * @param n value to insert
     */
    void insert_in_numerical_order(std::vector<int>& v, int n)
    {
        if(v.empty()) {
            v.push_back(n);
            return;
        }

        auto it = v.begin();
        while(it != v.end() && *it < n) { ++it; }
        v.insert(it, n);
    }

    /**
     * @brief insert n into l in numerical order
     *
     * @param l list to insert into
     * @param n value to insert
     */
    void insert_in_numerical_order(std::list<int>& l, int n)
    {
        if(l.empty()) {
            l.push_back(n);
            return;
        }

        auto it = l.begin();
        while(it != l.end() && *it < n) { ++it; }
        l.insert(it, n);
    }

} // namespace utils

/**
 * @brief abstract base class for a sequence of integers
 */
class IntegerSequence {
public:
    /**
     * @brief insert n into the sequence in numerical order
     *
     * @param n the value to insert
     */
    virtual void insert_numerical(int n) = 0;
    /**
     * @brief push n onto the end of the sequence
     *
     * @param n the value to push
     */
    virtual void push_back(int n) = 0;
    /**
     * @brief push n onto the front of the sequence
     *
     * @param n the value to push
     */
    virtual void push_front(int n) = 0;
    /**
     * @brief remove the nth element from the sequence
     *
     * @param i the index of the element to remove
     */
    virtual void remove(size_t i) = 0;
    /**
     * @brief return the number of elements in the sequence
     *
     * @return size_t the number of elements in the sequence
     */
    virtual size_t size() = 0;
    /**
     * @brief return true if the sequence is empty
     *
     * @return true if the sequence is empty
     * @return false if the sequence is not empty
     */
    virtual bool empty() = 0;
    virtual ~IntegerSequence() = default;

    /**
     * @brief fill the sequence incrementally with the values in s in numerical
     * order
     *
     * @param s the set of values to insert
     * @param num_vals the number of values to insert
     *
     * @note assert(s.size() >= num_vals)
     */
    void fill_numerically(std::unordered_set<int> s, size_t num_vals)
    {
        assert(s.size() >= num_vals);

        auto itr = s.begin();
        for(size_t i = 0; i < num_vals; ++i) {
            insert_numerical(*itr);
            ++itr;
        }
    }
};

/**
 * @brief Adaptor class for list<int> to IntegerSequence
 */
class ListAdaptor : public IntegerSequence {
private:
    std::list<int> l;
public:
    explicit ListAdaptor(std::list<int> const& l): l(l) {}
    ListAdaptor() = default;
    void insert_numerical(int n) override
    {
        utils::insert_in_numerical_order(l, n);
    }
    void push_back(int n) override { l.push_back(n); }
    void push_front(int n) override { l.push_front(n); }
    void remove(size_t i) override
    {
        auto it = l.begin();
        for(size_t j = 0; j < i; ++j) { ++it; }
        l.erase(it);
    }
    size_t size() override { return l.size(); }
    bool empty() override { return l.empty(); }
    ~ListAdaptor() override = default;
};

/**
 * @brief Adaptor class for vector<int> to IntegerSequence
 */
class VectorAdaptor : public IntegerSequence {
private:
    std::vector<int> v;
public:
    explicit VectorAdaptor(std::vector<int> const& v): v(v) {}
    VectorAdaptor() = default;
    void insert_numerical(int n) override
    {
        utils::insert_in_numerical_order(v, n);
    }
    void push_back(int n) override { v.push_back(n); }
    void push_front(int n) override { v.insert(v.begin(), n); }
    void remove(size_t i) override { v.erase(v.begin() + i); }
    size_t size() override { return v.size(); }
    bool empty() override { return v.empty(); }
    ~VectorAdaptor() override = default;
};

extern std::pair<std::chrono::nanoseconds, std::chrono::nanoseconds>
test_n(size_t num_vals, size_t num_runs = DEFAULT_RUNS_PER_TEST);

#endif // LVV_H