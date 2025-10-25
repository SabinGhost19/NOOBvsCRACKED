## C String Safety Guide


**Key Concepts:**
- Buffer overflow prevention
- Safe string functions (`strncpy`, `strncat`)
- Memory management best practices
- Input validation with `fgets`

**Resources:**
- [GNU C Library Manual - String Utilities](https://www.gnu.org/software/libc/manual/html_node/String-and-Array-Utilities.html)
- [SEI CERT C Coding Standard - String Handling](https://wiki.sei.cmu.edu/confluence/display/c/STR31-C)
- [Reddit r/C_Programming FAQ - Strings](https://www.reddit.com/r/C_Programming/wiki/faq/#wiki_strings)


> **Core concept**: Strings in C are just arrays of characters ending with a null terminator. The language provides no automatic safety checks—you are entirely responsible for preventing memory corruption.

## Why This Matters

C doesn't protect you from mistakes. Poor string handling causes crashes, security vulnerabilities, memory leaks, and unpredictable behavior. These problems are among the most common sources of critical bugs in C programs.

---

## The 8 Essential Rules

### 1. **Always Check Buffer Sizes Before Copying**

Before copying any string data, verify that the destination buffer is large enough to hold the source data plus the null terminator. Writing beyond buffer boundaries corrupts adjacent memory, leading to crashes or exploitable security holes known as buffer overflow vulnerabilities.

---

### 2. **Use strncpy/strncat Instead of strcpy/strcat**

The bounded versions of string functions let you specify the maximum number of bytes to copy, preventing writes beyond your buffer's end. The unbounded versions (strcpy, strcat) keep copying until they find a null terminator, regardless of your buffer size—a recipe for disaster with untrusted or long input.

---

### 3. **Always Null-Terminate Strings Explicitly**

The null terminator (the byte with value zero) is the "stop sign" that tells string functions where your string ends. Functions like strncpy don't guarantee null-termination if the source is too long. Without this terminator, functions like strlen and printf will read past your buffer into garbage memory, causing unpredictable behavior or crashes.

---

### 4. **Free Dynamically Allocated Memory**

Every memory allocation with malloc, calloc, or realloc must be matched with a corresponding free call. Memory leaks occur when allocated memory is never released—these leaks accumulate over time and eventually exhaust available memory. After freeing, set pointers to NULL to avoid dangling pointer bugs, where you accidentally use a pointer to memory that's already been released.

---

### 5. **Use fgets Instead of gets/scanf for Input**

The gets function was so dangerous it was removed from the C standard—it has no way to know your buffer size and will happily overflow it. Similarly, scanf with "%s" doesn't check bounds. The fgets function requires you to specify the maximum bytes to read, making buffer overflow impossible. Note that fgets includes the newline character, which you'll typically want to remove.

---

### 6. **Consider Using snprintf for Formatted Strings**

When building strings from multiple pieces of data, sprintf has no protection against buffer overflow. The snprintf variant takes a size parameter and guarantees it won't write beyond that limit. It always null-terminates the result and returns the number of characters that would have been written, allowing you to detect when truncation occurred.

---

### 7. **Validate Pointers Before Dereferencing**

Dereferencing a NULL pointer causes an immediate segmentation fault and program crash. Pointers can be NULL when malloc fails due to insufficient memory, when functions return NULL to indicate errors, or when variables are uninitialized. Always check that a pointer is not NULL before using it to access memory.

---

### 8. **Initialize Buffers to Prevent Undefined Behavior**

Uninitialized memory contains whatever random data was previously stored there—potentially sensitive information from other programs or invalid values that cause subtle bugs. Zero-initializing buffers ensures they start in a known, predictable state, makes debugging easier, and prevents accidental information leaks. This is especially important for string buffers, where garbage bytes could be misinterpreted as string content.

---
