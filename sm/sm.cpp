/**
 * @file sm.cpp
 * @brief Simple measurements and comparisons
 * @author Noam Bechhofer
 *
 * @details
 *
 * - Compare the times needed for loops: traditional C-style loops, Range for
 *   loops, and std::for_each() (with a lambda as the “loop body”), and
 *   std::accumulate.
 * – Fill a std::vector<int> with random numbers (use <random>), then use that
 * to measure the time needed for an accumulation, e.g.:
 *
 * ``` cpp
 * double sum = 0;
 * for (int i=0: i<max; ++i) sum+=v[i];
 * ```
 *
 * – Try a few variations, such as using an unsigned loop variable and
 * accumulating the sqrt(abs(v[i]))s.
 *
 * – Use <chrono> for measurements.
 *
 * – Be careful that the optimizer doesn’t eliminate your code (e.g., produce
 * and output a result of the computation you measure).
 *
 * – Future exercises will involve measurements, so organize your code so that
 * code to be measured can be easily “dropped in.”
 *
 * – Be sure to use the optimizer (e.g. -O2 or –O3 or "release").
 *
 * – Run each test at least three times and compare the results (to protect
 * against interference from other activities on your computer).
 *
 * – Run examples with sufficient data to get meaningful results (at least many
 *   milliseconds).
 *
 * – Write a note explaining (1) your general strategy for writing and running
 *   the tests, (2) what conclusions you can draw from the data generated, and
 *   (3) what surprises (if any) you got along the way.
 */

#include <algorithm>
#include <atomic>
#include <cassert>
#include <chrono>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <numeric>
#include <random>
#include <vector>

using namespace std;

constexpr size_t NUM_RUNS = 10;
constexpr size_t VECTOR_SIZE = 1'000'000'000;

const vector<pair<string, function<int(std::vector<int> const&)>>>
    test_functions
    = {{"c-style",
        [](const vector<int>& v) {
            int sum = 0;
            assert(v.size() <= INT_MAX);
            auto stop = (int)v.size();
            for(int i = 0; i < stop; i = i + 1) sum += v[i];
            return sum;
        }},
       {"c-style unsigned",
        [](const vector<int>& v) {
            int sum = 0;
            auto stop = v.size();
            for(unsigned int i = 0; i < stop; ++i) sum += v[i];
            return sum;
        }},
       {"range for loop",
        [](const vector<int>& v) {
            int sum = 0;
            for(const auto& i: v) sum += i;
            return sum;
        }},
       {"for_each",
        [](const vector<int>& v) {
            int sum = 0;
            for_each(v.begin(), v.end(), [&sum](int i) { sum += i; });
            return sum;
        }},
       {"ranges::for_each",
        [](const vector<int>& v) {
            int sum = 0;
            ranges::for_each(v, [&sum](int i) { sum += i; });
            return sum;
        }},
       {"accumulate",
        [](const vector<int>& v) { return accumulate(v.begin(), v.end(), 0); }},
       {"accumulate sqrt abs", [](const vector<int>& v) {
            return accumulate(v.begin(), v.end(), 0,
                              [](int a, int b) { return a + sqrt(abs(b)); });
        }}};

/**
 * @brief Time how long it takes to run a given callable object (function,
 *        lambda, etc.)
 *
 * @tparam F type of the callable object
 * @tparam Args argument types of the callable object
 * @param f callable object to run
 * @param args arguments to pass to the callable object
 * @return time it took to run the callable object
 */
template<class F, class... Args> auto time_test(F f, Args... args)
    -> std::pair<chrono::milliseconds, std::invoke_result_t<F, Args...>>
{
    auto start = chrono::high_resolution_clock::now();
    auto result = f(args...);
    auto end = chrono::high_resolution_clock::now();
    return {chrono::duration_cast<chrono::milliseconds>(end - start), result};
}

void run_tests(ostream& out = cout, ostream& err = cerr)
{
    mt19937 gen(random_device{}());
    uniform_int_distribution dist(INT_MIN, INT_MAX);

    vector<int> v{VECTOR_SIZE};

    err << "filling vector with " << VECTOR_SIZE
        << " random numbers, this might take a while\n";
    // volatile to discourage the optimizer from eliminating the loop
    for(volatile size_t i = 0; i < VECTOR_SIZE; i = i + 1) {
        v.emplace_back(dist(gen));
    }
    err << "done, running tests\n";

    for(const auto& [name, f]: test_functions) {
        out << std::left << std::setw(25) << name << "\t";
        auto total_time = chrono::milliseconds(0);
        for(size_t i = 0; i < NUM_RUNS; ++i) {
            auto [time, result] = time_test(f, v);
            total_time += time;
            err << name << " run # " << i << " done, took " << time.count()
                << "ms, result: " << result << "\n";
        }
        out << (total_time.count() / NUM_RUNS) << "ms\t\n";
    }

    out << flush;
}

int main(void)
{
    run_tests();
    return 0;
}