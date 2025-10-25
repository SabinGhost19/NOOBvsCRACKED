
/* ============================================
 * NOOB VERSION - Common Mistakes
 * ============================================ */

void noob_string_copy() {

  // PROBLEM 1: No bounds checking
  char dest[10];
  char *src = "This is a very long string that will overflow";
  strcpy(dest, src); // BUFFER OVERFLOW! Undefined behavior
  printf("Copied (dangerous): %s\n", dest);

  // PROBLEM 2: No memory management
  char *str1 = malloc(100);
  strcpy(str1, "Hello");
  // Forgot to free! Memory leak

  // PROBLEM 3: Uninitialized buffer
  char buffer[50];
  strcat(buffer, "World"); // Undefined behavior - buffer not initialized

  // PROBLEM 4: Hardcoded sizes
  char name[20];
  printf("Enter name: ");
  scanf("%s", name); // No size limit, can overflow
}

/* ============================================
 * CRACKED VERSION - Best Practices
 * ============================================ */

// Helper function: Safe string copy with error handling
int safe_string_copy(char *dest, size_t dest_size, const char *src) {
  if (!dest || !src || dest_size == 0) {
    // invalid parameters
    return -1;
  }

  size_t src_len = strlen(src);
  if (src_len >= dest_size) {
    // source too large
    return -1;
  }

  strncpy(dest, src, dest_size - 1);
  // ensure null termination
  dest[dest_size - 1] = '\0';
  return 0;
}

// Helper function: Dynamic string allocation
char *create_string_copy(const char *src) {
  if (!src) {
    return NULL;
  }

  size_t len = strlen(src);
  // +1 for null terminator
  char *copy = malloc(len + 1);

  if (!copy) {
    fprintf(stderr, "Memory allocation failed\n");
    return NULL;
  }
  // copy including null terminator
  memcpy(copy, src, len + 1);
  return copy;
}

void cracked_string_handling() {
  printf("\n=== CRACKED VERSION ===\n");

  //~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~//
  // SOLUTION 1: Bounds checking with strncpy
  char dest[10];
  const char *src = "This is a very long string that will overflow";

  if (safe_string_copy(dest, sizeof(dest), src) == -1) {
    printf("String too long, truncated safely\n");
  } else {
    printf("Copied safely: %s\n", dest);
  }
  //~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~//
  // SOLUTION 2: Proper memory management
  char *str1 = create_string_copy("Hello");
  if (str1) {
    printf("Dynamic string: %s\n", str1);
    // always free allocated memory
    free(str1);
    // avoid dangling pointer
    str1 = NULL;
  }

  //~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~//
  // SOLUTION 3: Initialize buffers

  // zero-initialize
  char buffer[50] = {0};
  strncat(buffer, "World", sizeof(buffer) - strlen(buffer) - 1);
  printf("Concatenated safely: %s\n", buffer);

  //~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~//
  // SOLUTION 4: Use fgets for safe input with size limit
  char name[20];
  printf("Enter name: ");
  if (fgets(name, sizeof(name), stdin)) {
    // remove trailing newline if present
    size_t len = strlen(name);
    if (len > 0 && name[len - 1] == '\n') {
      name[len - 1] = '\0';
    }
    printf("Name entered: %s\n", name);
  }
}
