/**
 * @file lvv.cpp
 * @brief Compare the performance of list<int> and vector<int>
 * @author Noam Bechhofer
 *
 * @details
 * # list v. vector
 *
 * - Exercise (as posed to me by John Bentley many years ago):
 * - Generate N random integers and insert them into a sequence so that each is
 *   inserted in its proper position in the numerical order. **5 1 4 2** gives:
 *         - 5
 *         - 1 5
 *         - 1 4 5
 *         - 1 2 4 5
 * - Remove elements one at a time by picking a random position in the sequence
 *   and removing the element there. Positions **1 2 0 0** gives
 *         - 1 2 4 5
 *         - 1 4 5
 *         - 1 4
 *         - 4
 * - For which N is it better to use a linked list than a vector (or an array)
 *   to represent the sequence?
 * - The sequence grows incrementally
 * - Use **vector** and **list**. Use the same algorithms for both vector
 *   and list. Do not use **advance()** to traverse a sequence, write an
 *   explicit loop (**advance()** will do optimizations that will perturb your
 *   results). The N random numbers should come from a uniform distribution, but
 *   the numbers need to be distinct (i.e., duplicate values not are allowed;
 *   that implies that you’ll have to generate the sequence first and then use
 *   it. That also saves you from the mistake of measuring the cost of
 *   generating the initial random sequence as part of inserting and removing
 *   its elements from the ordered sequence).
 * - Use **<random>** to generate random integers and **<chrono>** for timing.
 *   Run each experiment three times with different seeds for the random number
 *   generator to make it likely that your numbers are not perturbed by external
 *   factors.
 * - Draw a graph of the times for various numbers of elements (e.g., for 100K,
 *   200K, … elements). You can hand-draw if you don’t have a suitable way of
 *   drawing from a program.
 * - Do the experiment again using a **set**, and add its performance to the
 *   graph (to see the effect of using an O(log n) algorithm).
 * - Run at least one complementary experiment to help you understand what you
 *   see. For example, store the integers in a large struct to see the effects
 *   of size, pre-allocate the list elements to see the cost of allocation, or
 *   something.
 * - Write an explanation of why your results are the way they are – this
 *   write-up is essential, do not just send code and a graph.
 * - The code needs to be submitted together with the results. Try not to write
 *   C or Java in C++ - if you do, you’ll end up writing too much code.
 * - Due **Wednesday October 12**. I recommend to start early; people trying to
 *   do this exercise the last evening have often been unpleasantly surprised
 *   about the time needed.
 */

#include "lvv.h"
#include <cassert>
#include <chrono>
#include <climits>
#include <fstream>
#include <future>
#include <iostream>
#include <list>
#include <map>
#include <optional>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

constexpr int MAX_TESTS = 1'000'000;
constexpr int DEFAULT_NUM_TESTS = 10'000;

using namespace std;

namespace db {

    // NUM_INTS == MAX_TESTS is not a rule of nature, just how I did it
    constexpr int NUM_INTS = MAX_TESTS;
    const string INT_DB_REL_PATH = "./random_ints.txt";

    /**
     * @brief try to read a set of integers from the database
     *
     * @see #INT_DB_REL_PATH
     *
     * @return optional<unordered_set<int>> the set read from the database, or
     * nullopt if the database could not be opened
     */
    optional<unordered_set<int>> read_int_db()
    {
        ifstream int_db;
        int_db.open(INT_DB_REL_PATH);
        if(!int_db) { return {}; }

        unordered_set<int> s;
        s.reserve(NUM_INTS);

        string line;
        while(getline(int_db, line)) { s.insert(stoi(line)); }
        return s;
    }

} // namespace db

/**
 * @brief fetch a set of integers to use for testing
 *
 * @param min_num_ints the minimum number of integers to fetch
 * @return unordered_set<int> the set of integers to use for testing
 */
unordered_set<int> fetch_int_set(int min_num_ints)
{
    return db::read_int_db().value_or(
        utils::generate_n_random_ints(min_num_ints, INT_MIN, INT_MAX));
}
/** A set of random integers */
const unordered_set<int> INT_SET = fetch_int_set(db::NUM_INTS);

/**
 * @brief INTERNAL: the timed test function. The caller will be timing this
 * function, so it should not do any blocking behavior
 *
 * @param removal_indices a vector dictating the order in which to remove
 * elements.
 * @details removal_indices[i] is the index of the element to remove on the ith
 *          iteration. So for example, given sequence {1, 2, 4, 5} and removal
 *          indices {1, 2, 0, 0}, the sequence would be reduced to {1, 4, 5},
 *          then {1, 4}, then {4}, then {}.
 *          Note that the size of removal_indices indicates the number of
 *          values that will be inserted and removed.
 */
void inline test_n_core_(IntegerSequence& seq,
                         const vector<size_t>& removal_indices)
{
    const size_t num_vals = removal_indices.size();
    assert(INT_SET.size() >= num_vals);

    auto itr = INT_SET.begin();
    for(size_t i = 0; i < num_vals; i++) { seq.insert_numerical(*itr++); }
    for(size_t i: removal_indices) { seq.remove(i); }
}

/**
 * @brief INTERNAL: driver function for #test_n. Runs synchronously
 * (directly returns the result) and asynchronously (sets the result on the
 * supplied promise)
 *
 * @param seq the sequence to test
 * @param num_vals the number of values to insert and remove
 * @param promise the promise to set the result on
 * @param num_runs how many times to run the test
 *
 * @return chrono::nanoseconds the average time it took to run the test
 */
chrono::nanoseconds test_n_(IntegerSequence& seq, size_t num_vals,
                            promise<chrono::nanoseconds> promise,
                            size_t num_runs = DEFAULT_RUNS_PER_TEST)
{
    chrono::nanoseconds avg{0};
    for(size_t i = 0; i < num_runs; ++i) {
        // I don't think reseeding is necessary but prof. wants us to do it
        gen.seed(random_device{}());

        vector<size_t> removal_indices{num_vals};
        for (size_t back = num_vals - 1; back < SIZE_MAX; back--) {
            removal_indices.emplace_back(utils::random_size_t(0, back));
        }

        // sanity check
        assert(removal_indices.at(removal_indices.size() - 1) == 0);

        auto start = chrono::high_resolution_clock::now();
        test_n_core_(seq, removal_indices);
        auto end = chrono::high_resolution_clock::now();

        avg += chrono::duration_cast<chrono::nanoseconds>(end - start);
    }

    chrono::nanoseconds result = avg / num_runs;
    promise.set_value(result);
    return result;
}

/**
 * @brief test the performance of a vector and a list for a specific N by
 *        inserting and removing (in random order) num_vals elements
 *
 * @param num_vals number of elements to insert and remove
 * @param num_runs how many times to run the test
 *
 * @return pair<chrono::nanoseconds, chrono::nanoseconds> the average time it
 *         took to run the test for the vector and the list, respectively
 */
pair<chrono::nanoseconds, chrono::nanoseconds> test_n(size_t num_vals,
                                                      size_t num_runs = DEFAULT_RUNS_PER_TEST)
{
    assert(INT_SET.size() >= num_vals);

    VectorAdaptor v{};
    ListAdaptor l{};

    promise<chrono::nanoseconds> vec_promise;
    promise<chrono::nanoseconds> list_promise;

    future<chrono::nanoseconds> vec_future = vec_promise.get_future();
    future<chrono::nanoseconds> list_future = list_promise.get_future();

    jthread vec_thread(test_n_, ref(v), num_vals, move(vec_promise), num_runs);
    jthread list_thread(test_n_, ref(l), num_vals, move(list_promise), num_runs);

    chrono::nanoseconds vec_duration = vec_future.get();
    chrono::nanoseconds list_duration = list_future.get();

    return make_pair(vec_duration, list_duration);
}

/**
 * @brief perform #test_n for a range of values
 *
 * @param start the first value to test
 * @param end the last value to test
 * @param output the output stream to write to
 */
void test_block(size_t start, size_t end, ostream& output)
{
    for(size_t i = start; i < end; i++) {
        auto [vec_duration, list_duration] = test_n(i, DEFAULT_RUNS_PER_TEST);

        output << i << "," << vec_duration.count() << ","
               << list_duration.count() << ","
               << list_duration.count() - vec_duration.count() << endl;
    }
}

/**
 * @brief parse command line arguments and return the number of tests to run
 *
 * @param argv command line arguments
 * @return size_t the number of tests to run (#DEFAULT_NUM_TESTS if no argument
 *         is found)
 */
size_t lvv_parse_args(vector<string> argv)
{
    size_t argc = argv.size();
    if(argc < 1) {
        // to my understanding, this doesn't happen
        assert(false);
        exit(1);
    }
    if(argc == 1) { return DEFAULT_NUM_TESTS; }
    if(argc > 2) {
        cerr << "Usage: " << argv[0] << " [optional: number of tests to run]"
             << endl;
        exit(1);
    }
    assert(argc == 2);

    int tests = stoi(argv[1]);

    if(tests < 0) {
        cerr << "Number of tests must be non-negative" << endl;
        exit(1);
    }

    if(tests > MAX_TESTS) {
        cerr << "Number of tests must be less than " << MAX_TESTS << endl;
        exit(1);
    }

    return tests;
}

int main(int argc, char const* argv[])
{
    size_t num_tests = lvv_parse_args(vector<string>{argv, argv + argc});

    std::ofstream outfile("out.csv");
    outfile << "x,vectime,listtime,vecgain\n";

    test_block(0, num_tests, outfile);

    outfile.close();
}
