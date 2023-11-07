
/*
# list v. vector

- Exercise (as posed to me by John Bentley many years ago):
- Generate N random integers and insert them into a sequence so that each is
  inserted in its proper position in the numerical order. **5 1 4 2** gives:
        - 5
        - 1 5
        - 1 4 5
        - 1 2 4 5
- Remove elements one at a time by picking a random position in the sequence and
  removing the element there. Positions **1 2 0 0** gives
        - 1 2 4 5
        - 1 4 5
        - 1 4
        - 4
- For which N is it better to use a linked list than a vector (or an array) to
  represent the sequence?
- The sequence grows incrementally
- Use **vector** and **list**. Use the same algorithms for both vector
  and list. Do not use **advance()** to traverse a sequence, write an
  explicit loop (**advance()** will do optimizations that will perturb your
  results). The N random numbers should come from a uniform distribution, but
  the numbers need to be distinct (i.e., duplicate values not are allowed; that
  implies that you’ll have to generate the sequence first and then use it. That
  also saves you from the mistake of measuring the cost of generating the
  initial random sequence as part of inserting and removing its elements from
  the ordered sequence).
- Use **<random>** to generate random integers and **<chrono>** for timing. Run
  each experiment three times with different seeds for the random number
  generator to make it likely that your numbers are not perturbed by external
  factors.
- Draw a graph of the times for various numbers of elements (e.g., for 100K,
  200K, … elements). You can hand-draw if you don’t have a suitable way of
  drawing from a program.
- Do the experiment again using a **set**, and add its performance to the
  graph (to see the effect of using an O(log n) algorithm).
- Run at least one complementary experiment to help you understand what you see.
  For example, store the integers in a large struct to see the effects of size,
  pre-allocate the list elements to see the cost of allocation, or something.
- Write an explanation of why your results are the way they are – this write-up
  is essential, do not just send code and a graph.
- The code needs to be submitted together with the results. Try not to write C
  or Java in C++ - if you do, you’ll end up writing too much code.
- Due **Wednesday October 12**. I recommend to start early; people trying to do
  this exercise the last evening have often been unpleasantly surprised about
  the time needed.
*/

#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <random>
#include <string>
#include <unordered_set>
#include <vector>

constexpr int NUM_TESTS = 1'000'000;

using namespace std;

namespace utils {


/**
 * @brief generate a uniformly distributed random integer between min and max (inclusive)
 *
 * @param min minimum value (inclusive)
 * @param max maximum value (inclusive)
 * @return int a random integer between min and max (inclusive)
 */
int random_int(int min, int max, mt19937& gen)
{
    auto dist = uniform_int_distribution<>(min, max);
    return dist(gen);
}

/**
 * @brief generate a set of n uniformly distributed random integers between min and max (inclusive)
 *
 * @param n number of integers to generate
 * @param min minimum value (inclusive)
 * @param max maximum value (inclusive)
 * @param gen random number generator
 * @return set<int> a set of n uniformly distributed random integers between min and max (inclusive)
 */
unordered_set<int> generate_n_random_ints(size_t n, int min, int max, mt19937& gen)
{
    unordered_set<int> s;
    while (s.size() < n) {
        s.insert(random_int(min, max, gen));
    }
    return s;
}

/**
 * @brief insert n into v in numerical order
 *
 * @param v vector to insert into
 * @param n value to insert
 */
void insert_in_numerical_order(vector<int>& v, int n)
{
    if (v.empty()) {
        v.push_back(n);
        return;
    }
    auto it = v.begin();
    while (it != v.end() && *it < n) {
        ++it;
    }
    v.insert(it, n);
}

/**
 * @brief insert n into l in numerical order
 *
 * @param l list to insert into
 * @param n value to insert
 */
void insert_in_numerical_order(list<int>& l, int n)
{
    if (l.empty()) {
        l.push_back(n);
        return;
    }
    auto it = l.begin();
    while (it != l.end() && *it < n) {
        ++it;
    }
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
     * @brief fill the sequence incrementally with the values in s in numerical order
     *
     * @param s the set of values to insert
     * @param num_vals the number of values to insert
     *
     * @note assert(s.size() >= num_vals)
     */
    void fill_numerically(unordered_set<int> s, size_t num_vals)
    {
        assert(s.size() >= num_vals);

        auto itr = s.begin();
        for (size_t i = 0; i < num_vals; ++i) {
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
    list<int> l;

public:
    explicit ListAdaptor(list<int> const& l)
        : l(l)
    {
    }
    ListAdaptor() = default;
    void insert_numerical(int n) override
    {
        utils::insert_in_numerical_order(l, n);
    }
    void push_back(int n) override
    {
        l.push_back(n);
    }
    void push_front(int n) override
    {
        l.push_front(n);
    }
    void remove(size_t i) override
    {
        auto it = l.begin();
        advance(it, i);
        l.erase(it);
    }
    size_t size() override
    {
        return l.size();
    }
    bool empty() override
    {
        return l.empty();
    }
    ~ListAdaptor() override = default;
};

/**
 * @brief Adaptor class for vector<int> to IntegerSequence
 */
class VectorAdaptor : public IntegerSequence {
private:
    vector<int> v;

public:
    explicit VectorAdaptor(vector<int> const& v)
        : v(v)
    {
    }
    VectorAdaptor() = default;
    void insert_numerical(int n) override
    {
        utils::insert_in_numerical_order(v, n);
    }
    void push_back(int n) override
    {
        v.push_back(n);
    }
    void push_front(int n) override
    {
        v.insert(v.begin(), n);
    }
    void remove(size_t i) override
    {
        auto it = v.begin();
        advance(it, i);
        v.erase(it);
    }
    size_t size() override
    {
        return v.size();
    }
    bool empty() override
    {
        return v.empty();
    }
    ~VectorAdaptor() override = default;
};

namespace db {

constexpr int NUM_INTS = 1'000'000;
const string INT_DB_REL_PATH = "./random_ints.txt";

unordered_set<int> read_int_db()
{
    ifstream int_db;
    int_db.open(INT_DB_REL_PATH);
    if (!int_db) {
        cerr << "Unable to open integer database " << INT_DB_REL_PATH << endl;
        exit(1);
    }

    unordered_set<int> s;
    s.reserve(NUM_INTS);

    string line;
    while (getline(int_db, line)) {
        s.insert(stoi(line));
    }
    return s;
}

}

void inline _test(IntegerSequence& seq, size_t test_num, unordered_set<int> s, mt19937& gen)
{
    auto itr = s.begin();
    for (size_t i = 0; i < test_num; ++i) {
        seq.insert_numerical(*itr++);
    }

    while (!seq.empty()) {
        size_t i = utils::random_int(0, (int)(seq.size() - 1), gen);
        seq.remove(i);
    }
}

constexpr size_t NUM_RUNS = 3;
chrono::nanoseconds test(IntegerSequence& seq, size_t test_num, unordered_set<int> const& s, mt19937& gen)
{
    /* first do a warmup... */
    _test(seq, test_num, s, gen);
    /* ... ok, now we measure */
    chrono::nanoseconds avg(0);
    for (size_t i = 0; i < NUM_RUNS; ++i) {
        // reseed the random number generator
        gen.seed(random_device{}());
        auto start = chrono::high_resolution_clock::now();
        _test(seq, test_num, s, gen);
        auto end = chrono::high_resolution_clock::now();
        avg += chrono::duration_cast<chrono::nanoseconds>(end - start);
    }
    return avg / NUM_RUNS;
}

int main(int argc, char const* argv[])
{
    size_t num_tests;
    if (argc < 1) {
        assert(false);
    } else if (argc == 1) {
        num_tests = NUM_TESTS;
    } else if (argc == 2) {
        num_tests = atoi(argv[1]);
    } else /* argc > 2 */ {
        cerr << "Usage: " << argv[0] << " [optional: number of tests to run]" << endl;
        exit(1);
    }

    mt19937 gen(random_device{}());

    /* Time to fetch our set of ints. Ideally there is a database available */
    unordered_set<int> s;
    ifstream int_db;
    int_db.open(db::INT_DB_REL_PATH);
    if (int_db) {
        s = unordered_set<int>(db::read_int_db());
    } else {
        s = utils::generate_n_random_ints(db::NUM_INTS, INT_MIN, INT_MAX, gen);
    }
    for (size_t i = 1563; i < num_tests; ++i) {
        auto v = VectorAdaptor();
        auto l = ListAdaptor();

        chrono::nanoseconds vec_duration = test(v, i, s, gen);
        chrono::nanoseconds list_duration = test(l, i, s, gen);

        chrono::nanoseconds diff_nano = chrono::abs(vec_duration - list_duration);
        chrono::microseconds diff_micro = chrono::duration_cast<chrono::microseconds>(diff_nano);
        chrono::milliseconds diff_milli = chrono::duration_cast<chrono::milliseconds>(diff_nano);
        chrono::seconds diff_seconds = chrono::duration_cast<chrono::seconds>(diff_nano);

        cout << "vector\twith N = " << i << ":\t" << vec_duration.count() << "\tns\n";
        cout << "list\twith N = " << i << ":\t" << list_duration.count() << "\tns\n";

        cout << "for N = " << i << ",\t"
             << ((vec_duration < list_duration) ? "vector " : "list ")
             << "\twins by\t"
             << diff_nano.count() << " ns,\t"
             << diff_micro.count() << " us,\t"
             << diff_milli.count() << " ms,\t"
             << diff_seconds.count() << " s\n"
             << "\n";
    }
}
