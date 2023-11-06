/*
- Compare the times needed for loops: traditional C-style loops, Range for
  loops, and std::for_each() (with a lambda as the “loop body”), and
  std::accumulate.
– Fill a std::vector<int> with random numbers (use <random>), then use that to
  measure the time needed for an accumulation, e.g.:

```cpp
double sum = 0;
for (int i=0: i<max; ++i) sum+=v[i];
```

– Try a few variations, such as using an unsigned loop variable and accumulating
  the sqrt(abs(v[i]))s.

– Use <chrono> for measurements.

– Be careful that the optimizer doesn’t eliminate your code (e.g., produce and
  output a result of the computation you measure).

– Future exercises will involve measurements, so organize your code so that code
  to be measured can be easily “dropped in.”

– Be sure to use the optimizer (e.g. -O2 or –O3 or “release”).

– Run each test at least three times and compare the results (to protect against
  interference from other activities on your computer).

– Run examples with sufficient data to get meaningful results (at least many
  milliseconds).

– Write a note explaining (1) your general strategy for writing and running the
  tests, (2) what conclusions you can draw from the data generated, and (3) what
  surprises (if any) you got along the way.
*/

#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <numeric>
#include <random>
#include <vector>

using namespace std;

constexpr int NUM_RUNS = 10;
constexpr int VECTOR_SIZE = 1'000'000'000;

namespace tests {

chrono::nanoseconds test_c_style(vector<int>& v)
{
    auto start = chrono::high_resolution_clock::now();
    int sum = 0;
    for (int i = 0; i < (int)v.size(); ++i)
        sum += v[i];
    auto end = chrono::high_resolution_clock::now();
    cerr << sum << endl;
    return chrono::duration_cast<chrono::nanoseconds>(end - start);
}

chrono::nanoseconds test_c_style_unsigned(vector<int>& v)
{
    auto start = chrono::high_resolution_clock::now();
    int sum = 0;
    for (unsigned int i = 0; i < v.size(); ++i)
        sum += v[i];
    auto end = chrono::high_resolution_clock::now();
    cerr << sum << endl;
    return chrono::duration_cast<chrono::nanoseconds>(end - start);
}

chrono::nanoseconds test_range_for_loop(vector<int>& v)
{
    auto start = chrono::high_resolution_clock::now();
    int sum = 0;
    for (auto& i : v)
        sum += i;
    auto end = chrono::high_resolution_clock::now();
    cerr << sum << endl;
    return chrono::duration_cast<chrono::nanoseconds>(end - start);
}

chrono::nanoseconds test_for_each(vector<int>& v)
{
    auto start = chrono::high_resolution_clock::now();
    int sum = 0;
    for_each(v.begin(), v.end(), [&sum](int i) { sum += i; });
    auto end = chrono::high_resolution_clock::now();
    cerr << sum << endl;
    return chrono::duration_cast<chrono::nanoseconds>(end - start);
}

chrono::nanoseconds test_accumulate(vector<int>& v)
{
    auto start = chrono::high_resolution_clock::now();
    int sum = accumulate(v.begin(), v.end(), 0);
    auto end = chrono::high_resolution_clock::now();
    cerr << sum << endl;
    return chrono::duration_cast<chrono::nanoseconds>(end - start);
}

chrono::nanoseconds test_accumulate_sqrt_abs(vector<int>& v)
{
    auto start = chrono::high_resolution_clock::now();
    int sum = accumulate(v.begin(), v.end(), 0, [](int a, int b) {
        return a + sqrt(abs(b));
    });
    auto end = chrono::high_resolution_clock::now();
    cerr << sum << endl;
    return chrono::duration_cast<chrono::nanoseconds>(end - start);
}

/*
 * this is error prone, but I don't want to use a macro and C++ doesn't have
 * reflection
 */
vector<pair<string, function<chrono::nanoseconds(std::vector<int>&)>>> test_functions = {
    make_pair("C-style loop", test_c_style),
    make_pair("C-style loop (unsigned)", test_c_style_unsigned),
    make_pair("range for loop", test_range_for_loop),
    make_pair("for_each", test_for_each),
    make_pair("accumulate", test_accumulate),
    make_pair("accumulate (sqrt(abs()))", test_accumulate_sqrt_abs),
};

} // namespace tests

void run_tests()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 100);

    cerr << "this might take a while.\n";
    std::vector<int> v;
    cerr << "filling vector" << flush;
    for (int n = 0; n < VECTOR_SIZE; ++n) {
        if (n % (VECTOR_SIZE / 100) == 0)
            cerr << "." << flush;
        ;
        v.push_back(dis(gen));
    }
    cerr << "\ndone. vector size: " << v.size() << endl;

    for (auto& test : tests::test_functions) {
        chrono::nanoseconds sum = chrono::nanoseconds::zero();
        for (int i = 0; i < NUM_RUNS; ++i) {
            cerr << "running " << test.first << " iteration " << i + 1 << "/" << NUM_RUNS << "\n";
            sum += test.second(v);
        }
        chrono::nanoseconds avg = sum / NUM_RUNS;
        chrono::milliseconds avg_ms = chrono::duration_cast<chrono::milliseconds>(avg);
        cerr << test.first << " finished\n";
        cout << test.first << ":\t" << avg_ms.count() << " ms\n";
    }

    /*
    size_t num_tests = tests::test_functions.size();
    auto records = make_unique<chrono::nanoseconds[]>(num_tests);

    for (size_t i = 0; i < NUM_RUNS; i++) {
        for (size_t j = 0; j < num_tests; j++) {
            cerr << "running " << tests::test_functions[j].first << "\n";
            records[j] += tests::test_functions[j].second(v);
        }
        cerr << "iteration " << i + 1 << "/" << NUM_RUNS << "\n";
    }
    for (size_t i = 0; i < num_tests; i++) {
        records[i] /= NUM_RUNS;
        chrono::milliseconds avg_ms = chrono::duration_cast<chrono::milliseconds>(records[i]);
        cout << tests::test_functions[i].first << ":\t" << avg_ms.count() << " ms\n";
    }
    //*/
}

int main(void)
{
    run_tests();
    return 0;
}