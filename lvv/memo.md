<!-- Write an explanation of why your results are the way they are – this write-up is essential, do not just send code and a graph. -->

# List vs. Vector

Noam Bechhofer  
Design Using C++, Fall 2023, Prof. Stroustrup

## Executive Summary

The purpose of this experiment was to compare the performance of the `std::list` and `std::vector` containers in C++. The experiment was conducted by creating a program that would insert a large number of elements into a container, and then remove them randomly. The program was run multiple times, and the average time was recorded. The results of the experiment showed that the `std::vector` container was faster than the `std::list` container.

## Results

![fig 1](./Figure_1.svg)  
As can be seen in the graph above, `std::vector` is consistently faster than `std::list`. `std::list` also grows at a much faster rate than `std::vector` in relation to the number of elements inserted and removed.

## Analysis

Traditional wisdom says that when removing elements from the middle of a sequence, you should reach for a linked list. The idea goes like this: When you remove an element from the middle of a vector, you need to shunt all following (or preceding depending on implementation) elements over one space. We're told this is an O(n) operation. On the other hand, removing an element form a linked list is an O(1) operations - just redirect a pointer or two, and you're good.  
Traditional wisdom does not explain the results above, which is what makes this such an important experiment. While many programmers would immediately reach for a list given the constraints (knowing we're going to be removing from the middle), the results of this experiment show that a vector is actually faster.  
One confounding factor (though not the primary one) is cache locality. A vector is a contiguous block of memory, while a list is a bunch of scattered nodes. This means that a vector is more likely to be in the cache, and thus faster to access. However, if this were the whole story, we would expect to see some point where the linear cost of shunting all the elements over exceeds the constant time of a memory access. This is not the case.  
The primary issue is that to remove a node from the middle of a linked list we need to traverse the list node-by-node to just find the element. This is an O(n) operation, so even from a theoretical standpoint, we've already balanced out the cost of shunting elements over. And, as we can see from the graph. Combine this with vector's caching speedup, and we get the results we see above.

## Takeway for Developers

### The Importance of Measuring Performance

The experiment highlights a crucial aspect of programming: the importance of measuring performance. As developers, it's tempting to rely on conventional wisdom or theoretical assumptions when making decisions about data structures or algorithms. However, this experiment underscores the need for empirical evidence. The assumption that a `std::list` would be faster for middle-element removals was proven incorrect in practice. 

This leads us to an essential habit for developers: **being skeptical programmers**. Skepticism in programming doesn't mean doubting everything; rather, it means not accepting assertions without evidence, especially regarding performance. If you plan to deviate from standard practices or make optimizations, it is imperative to measure and validate your approach. Often, the intuitive choice isn't the most efficient one, as demonstrated in this assignment.

### Being a Skeptical Programmer

1. **Avoid Unsubstantiated Performance Claims**: Look for empirical evidence before making performance-related assertions. Theory and practice can diverge significantly.

2. **Focus on Functionality First**: Prioritize getting your system functional before delving into optimization. At best premature optimization can lead to unnecessary complexity, at worst it can be a footgun.

3. **Write Modular Code**: Develop code in a way that it can be easily optimized or refactored later.

4. **Thoughtful Abstraction Choices**: Select abstractions carefully. When in doubt, it might be better to delay abstracting until a clearer understanding is developed.

5. **Use Profiling Tools for Optimization**: When you do optimize, utilize profiling tools to test assumptions. Profiling can reveal bottlenecks you didn't expect, and show that bottlenecks you thought were significant were not a big deal.