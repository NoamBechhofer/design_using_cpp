<!--  Write a note explaining (1) your general strategy for writing and running the tests, (2) what conclusions you can draw from the data generated, and (3) what surprises (if any) you got along the way. -->
# Simple Measurement

Noam Bechhofer  
Design Using C++, Fall 2023, Prof. Stroustrup

## Strategy
The general testing strategy was to write a driver function `time_test` that takes a callable object and its args and then uses `<chrono>` to time the callable, returning a pair of the return value from the callable and the time it took to run.  
I then had this driver run 10 times through a vector of closures which each tested a different accumulation strategy, then calculated the average time for each strategy.

## Results
### 100,000,000 elements, 10 runs each
c-style                  	45ms	
c-style unsigned         	41ms	
range for loop           	40ms	
for_each                 	40ms	
ranges::for_each         	40ms	
accumulate               	40ms	
accumulate sqrt abs      	412ms
### 1,000,000,000 elements, 10 runs each
c-style                  	27159ms	
c-style unsigned         	42353ms	
range for loop           	43976ms	
for_each                 	59198ms	
ranges::for_each         	51315ms	
accumulate               	36413ms	
accumulate sqrt abs      	44673ms	


## Analysis
For the smaller vector, the results (besides accumulate sqrt abs) are all closely matched, which suggests to me that the overhead is more or less equivalent.  
For the larger vector (an increase of only 10x), the results are more varied. First, the unsigned loop is significantly slower than the signed loop. After a quick google, I [find](https://blog.llvm.org/2011/05/what-every-c-programmer-should-know.html#signed_overflow) that the compiler can make more optimizations for signed integers because their overflow behavior is undefined. Ranged for loops operate at about the same speed as unsigned c-style loops, which makes sense because range for loops are basically syntactic sugar for regular for loops. Between regular c-style loops and unsigned c-style loops sits `accumulate`. In my code, accumulate is passed an `int` as the third parameter allowing for signed integer optimizations, but it's likely that there is more overhead associated with `accumulate` than with a regular c-style loop. `accumulate` with sqrt and abs is slower, but the difference is much less pronounced now than the first test. Coming in last place we have `for_each` and `ranges::for_each`. Again, it isn't clear to me why the performance is so much worse. However, I now have to discuss the reliability of my results.

## Surprises
I'm surprised by the variance in runtimes. This variance also makes me skeptical of my results. See for example this sample run (results collected by stderr, 1,000,000,000 elements, 10 runs each; this is the run that yielded the second set of averages above):
```
c-style run # 0 done, took 22965ms, result: 155457589
c-style run # 1 done, took 60971ms, result: 155457589
c-style run # 2 done, took 54414ms, result: 155457589
c-style run # 3 done, took 20734ms, result: 155457589
c-style run # 4 done, took 41638ms, result: 155457589
c-style run # 5 done, took 24334ms, result: 155457589
c-style run # 6 done, took 16064ms, result: 155457589
c-style run # 7 done, took 9139ms, result: 155457589
c-style run # 8 done, took 12090ms, result: 155457589
c-style run # 9 done, took 9241ms, result: 155457589
c-style unsigned run # 0 done, took 35321ms, result: 155457589
c-style unsigned run # 1 done, took 12243ms, result: 155457589
c-style unsigned run # 2 done, took 60990ms, result: 155457589
c-style unsigned run # 3 done, took 72709ms, result: 155457589
c-style unsigned run # 4 done, took 41169ms, result: 155457589
c-style unsigned run # 5 done, took 15491ms, result: 155457589
c-style unsigned run # 6 done, took 49879ms, result: 155457589
c-style unsigned run # 7 done, took 29724ms, result: 155457589
c-style unsigned run # 8 done, took 63243ms, result: 155457589
c-style unsigned run # 9 done, took 42770ms, result: 155457589
range for loop run # 0 done, took 16226ms, result: 155457589
range for loop run # 1 done, took 6677ms, result: 155457589
range for loop run # 2 done, took 36222ms, result: 155457589
range for loop run # 3 done, took 66865ms, result: 155457589
range for loop run # 4 done, took 44768ms, result: 155457589
range for loop run # 5 done, took 60298ms, result: 155457589
range for loop run # 6 done, took 51599ms, result: 155457589
range for loop run # 7 done, took 10918ms, result: 155457589
range for loop run # 8 done, took 77142ms, result: 155457589
range for loop run # 9 done, took 69048ms, result: 155457589
for_each run # 0 done, took 39372ms, result: 155457589
for_each run # 1 done, took 55082ms, result: 155457589
for_each run # 2 done, took 61165ms, result: 155457589
for_each run # 3 done, took 67700ms, result: 155457589
for_each run # 4 done, took 20625ms, result: 155457589
for_each run # 5 done, took 91024ms, result: 155457589
for_each run # 6 done, took 72572ms, result: 155457589
for_each run # 7 done, took 43093ms, result: 155457589
for_each run # 8 done, took 76054ms, result: 155457589
for_each run # 9 done, took 65301ms, result: 155457589
ranges::for_each run # 0 done, took 68230ms, result: 155457589
ranges::for_each run # 1 done, took 38845ms, result: 155457589
ranges::for_each run # 2 done, took 29003ms, result: 155457589
ranges::for_each run # 3 done, took 37570ms, result: 155457589
ranges::for_each run # 4 done, took 71251ms, result: 155457589
ranges::for_each run # 5 done, took 43729ms, result: 155457589
ranges::for_each run # 6 done, took 59298ms, result: 155457589
ranges::for_each run # 7 done, took 65521ms, result: 155457589
ranges::for_each run # 8 done, took 55905ms, result: 155457589
ranges::for_each run # 9 done, took 43800ms, result: 155457589
accumulate run # 0 done, took 70106ms, result: 155457589
accumulate run # 1 done, took 40868ms, result: 155457589
accumulate run # 2 done, took 40634ms, result: 155457589
accumulate run # 3 done, took 51115ms, result: 155457589
accumulate run # 4 done, took 18212ms, result: 155457589
accumulate run # 5 done, took 37822ms, result: 155457589
accumulate run # 6 done, took 65384ms, result: 155457589
accumulate run # 7 done, took 13529ms, result: 155457589
accumulate run # 8 done, took 10174ms, result: 155457589
accumulate run # 9 done, took 16286ms, result: 155457589
accumulate sqrt abs run # 0 done, took 57467ms, result: 654755760
accumulate sqrt abs run # 1 done, took 39309ms, result: 654755760
accumulate sqrt abs run # 2 done, took 70201ms, result: 654755760
accumulate sqrt abs run # 3 done, took 13381ms, result: 654755760
accumulate sqrt abs run # 4 done, took 20216ms, result: 654755760
accumulate sqrt abs run # 5 done, took 51906ms, result: 654755760
accumulate sqrt abs run # 6 done, took 58746ms, result: 654755760
accumulate sqrt abs run # 7 done, took 53669ms, result: 654755760
accumulate sqrt abs run # 8 done, took 52449ms, result: 654755760
accumulate sqrt abs run # 9 done, took 29391ms, result: 654755760
```
I think this is due to OS context switching. If I had more time and access to resources I would try to run the tests on a dedicated machine to see if that would offer more uniform results. I also suspect parts of my vector are being memory optimized by my OS (Windows 11) because the task manager shows my process ranging between ~500 and ~3,900 MB of memory usage. By logic, 1,000,000,000 `int`s should take up 4,000,000,000 B = 4,000 MB of memory, and I am using the same vector for each test, so I conclude that NT is somehow optimizing memory usage at runtime. This probably also contributes to the variance in runtimes.  
So the immediate reaction to this would be to run more tests to get a better average. However, that last test took my computer an hour and 45 minutes to run, so I'm really coming up against my local machine's limits here.


## Takeaway for Developers
For most usecases (a billion length vector is exceedingly rare), the difference in speed between the different accumulation strategies is negligible. Therefore, I would recommend using whichever strategy is most readable and maintainable. I would only suggest using c-style loops when you've profiled a system and found the standard library alternatives to be a bottleneck, or if you really need to eke out every last bit of performance for a highly time-sensitive application.