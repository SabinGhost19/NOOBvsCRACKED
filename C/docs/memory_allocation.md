### C Memory Allocation


**Key Concepts:**
- Proper use of `malloc`, `calloc`, `realloc`
- Memory leak prevention
- Use-after-free and double-free bugs
- NULL pointer checking

**Resources:**
- [cppreference.com - C Memory Management](https://en.cppreference.com/w/c/memory)
- [Valgrind Documentation](https://valgrind.org/docs/manual/quick-start.html)
- ["Expert C Programming" by Peter van der Linden](https://www.amazon.com/Expert-Programming-Peter-van-Linden/dp/0131774298)
- [C Memory Management Reference](https://en.cppreference.com/w/c/memory) - Complete technical documentation
- [Why calloc vs malloc+memset](https://stackoverflow.com/questions/2688466/why-malloc-memset-is-slower-than-calloc) - Performance comparison
- [Memory Management Best Practices Thread](https://www.reddit.com/r/C_Programming/comments/8qm9f0/memory_management_best_practices/) - Community discussion
- [SEI CERT C Coding Standard: MEM00-C](https://wiki.sei.cmu.edu/confluence/display/c/MEM00-C) - Security-focused guidelines
- [Valgrind Quick Start Guide](https://valgrind.org/docs/manual/quick-start.html) - Tool documentation
- "Expert C Programming" by Peter van der Linden (Chapter 4) - Deep dive into memory



# C Memory Management Guide

> **Core Truth**: In C, you manually control every byte of dynamic memory. Mistakes cause crashes, corruption, and security holes.

## Why Memory Management Matters

Manual memory management gives C its speed but demands perfect discipline. Memory leaks waste resources. Use-after-free bugs corrupt data. Double-free errors crash programs. Buffer overflows create security vulnerabilities. These bugs are subtle, hard to debug, and often don't manifest immediately.

---

## The 10 Essential Rules

### 1. Always Check Allocation Return Values

`malloc`, `calloc`, and `realloc` return NULL when memory allocation fails. Using a NULL pointer crashes your program instantly. Always check before dereferencing. Handle the failure gracefully—log an error, clean up resources, and exit safely.

### 2. Free Every Allocated Pointer Exactly Once

Every allocation needs exactly one corresponding `free`. Missing `free` calls leak memory. Calling `free` twice on the same pointer corrupts the heap and causes crashes. Track your allocations carefully—if you `malloc` it, you must `free` it.

### 3. Set Pointers to NULL After Freeing

After `free(ptr)`, the pointer still holds the old address—it's now a "dangling pointer" to invalid memory. Setting `ptr = NULL` prevents accidentally using freed memory. Dereferencing NULL crashes immediately with a clear error; using freed memory causes subtle corruption.

### 4. Use calloc When You Need Zero-Initialized Memory

`calloc` allocates and zero-initializes memory in one operation. It's not just convenient—it's often faster than `malloc` plus `memset` due to operating system optimizations. Use `calloc` when you need clean memory; use `malloc` when you'll immediately overwrite it anyway.

### 5. Use realloc Carefully

`realloc` can fail, returning NULL while leaving the original pointer valid. Never write `ptr = realloc(ptr, new_size)` because you'll lose the original pointer if realloc fails, creating a memory leak. Always use a temporary variable to check the result first.

### 6. Create Cleanup Functions for Complex Structures

When structures contain multiple allocated pointers, create a dedicated cleanup function. This centralizes deallocation logic, prevents forgetting to free nested allocations, and makes your code more maintainable. It's the C equivalent of a destructor.

### 7. Use Tools Like Valgrind to Detect Leaks

Memory bugs are invisible during normal execution. Valgrind detects leaks, invalid accesses, and use-after-free errors by monitoring every memory operation. Run it regularly during development—don't wait until production failures force you to investigate.

### 8. Consider RAII-Like Patterns with Cleanup Functions

Use patterns where functions that allocate resources also provide cleanup functions. Pair `init` functions with `destroy` functions. This makes resource management explicit and helps ensure proper cleanup even when errors occur.

### 9. Match Every malloc with a free

This is the golden rule. Every allocation must have a corresponding deallocation. No exceptions. Track allocation/deallocation pairs in your code mentally or with comments. In complex code, consider keeping an allocation log during debugging.

### 10. Document Ownership of Pointers Clearly

When passing pointers between functions, document who's responsible for freeing them. Does the caller own it? Does the callee take ownership? Is it a borrowed reference? Unclear ownership causes double-frees or leaks. Make it explicit in comments and function documentation.


---

