/* ============================================
 * NOOB VERSION - Memory Leaks and Issues
 * ============================================ */

void noob_memory_allocation() {
  printf("\n=== NOOB VERSION ===\n");

  // PROBLEM 1: Memory leak - forgot to free
  int *numbers = malloc(10 * sizeof(int));
  for (int i = 0; i < 10; i++) {
    numbers[i] = i;
  }
  printf("Allocated array (leaked)\n");
  // Missing free(numbers)!

  // PROBLEM 2: Using freed memory (use-after-free)
  int *ptr = malloc(sizeof(int));
  *ptr = 42;
  free(ptr);
  printf("Value after free: %d\n", *ptr); // UNDEFINED BEHAVIOR!

  // PROBLEM 3: Double free
  int *data = malloc(sizeof(int));
  *data = 100;
  free(data);
  free(data); // CRASH! Double free

  // PROBLEM 4: No null check after malloc
  // might fail
  int *large = malloc(1000000000 * sizeof(int));
  // cRASH if malloc failed!
  *large = 5;

  // PROBLEM 5: Memory not initialized
  int *values = malloc(5 * sizeof(int));
  // garbage value
  printf("Uninitialized: %d\n", values[0]);
}

/* ============================================
 * CRACKED VERSION - Proper Memory Management
 * ============================================ */

// safe allocation wrapper with error checking
void *safe_malloc(size_t size) {
  void *ptr = malloc(size);
  if (!ptr && size != 0) {
    fprintf(stderr, "ERROR: malloc failed for %zu bytes\n", size);
    exit(EXIT_FAILURE);
  }
  return ptr;
}

// safe calloc wrapper (allocates and zeros memory)
void *safe_calloc(size_t count, size_t size) {
  void *ptr = calloc(count, size);
  if (!ptr && count != 0 && size != 0) {
    fprintf(stderr, "ERROR: calloc failed for %zu elements\n", count);
    exit(EXIT_FAILURE);
  }
  return ptr;
}

// safe realloc wrapper
void *safe_realloc(void *ptr, size_t new_size) {
  void *new_ptr = realloc(ptr, new_size);
  if (!new_ptr && new_size != 0) {
    fprintf(stderr, "ERROR: realloc failed for %zu bytes\n", new_size);
    // original pointer still valid, don't exit
    return NULL;
  }
  return new_ptr;
}

// Example structure with proper cleanup
typedef struct {
  int *data;
  size_t size;
  size_t capacity;
} DynamicArray;

DynamicArray *create_dynamic_array(size_t initial_capacity) {
  DynamicArray *arr = safe_malloc(sizeof(DynamicArray));
  arr->data = safe_calloc(initial_capacity, sizeof(int));
  arr->size = 0;
  arr->capacity = initial_capacity;
  return arr;
}

void destroy_dynamic_array(DynamicArray **arr) {
  if (arr && *arr) {
    free((*arr)->data);
    (*arr)->data = NULL; // prevent use-after-free
    free(*arr);
    *arr = NULL; // prevent double-free
  }
}

void cracked_memory_management() {
  printf("\n=== CRACKED VERSION ===\n");

  // SOLUTION 1: Always free allocated memory
  int *numbers = safe_malloc(10 * sizeof(int));
  for (int i = 0; i < 10; i++) {
    numbers[i] = i;
  }
  printf("Allocated array: ");
  for (int i = 0; i < 10; i++) {
    printf("%d ", numbers[i]);
  }
  printf("\n");
  free(numbers);
  numbers = NULL; // !!!!good practice

  //~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~//
  // SOLUTION 2: Don't use freed memory
  int *ptr = safe_malloc(sizeof(int));
  *ptr = 42;
  printf("Value before free: %d\n", *ptr);
  free(ptr);
  ptr = NULL; // set to NULL after freeing
  // now can safely check: if (ptr) { ... }

  //~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~//
  // SOLUTION 3: Avoid double free by setting to NULL
  int *data = safe_malloc(sizeof(int));
  *data = 100;
  free(data);
  data = NULL;
  // this is now safe (free(NULL) is a no-op)
  free(data);

  //~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~//
  // SOLUTION 4: Always check malloc return value
  size_t huge_size = 1000000000;
  int *large = malloc(huge_size * sizeof(int));
  if (large == NULL) {
    printf("Allocation failed gracefully\n");
  } else {
    *large = 5;
    free(large);
  }

  //~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~//
  // SOLUTION 5: Use calloc to zero-initialize
  int *values = safe_calloc(5, sizeof(int));
  printf("Initialized values: ");
  for (int i = 0; i < 5; i++) {
    printf("%d ", values[i]); // All zeros
  }
  printf("\n");
  free(values);

  //~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~=~//
  // SOLUTION 6: Use proper structures with cleanup
  DynamicArray *arr = create_dynamic_array(10);
  arr->data[0] = 100;
  arr->size = 1;
  printf("Dynamic array first element: %d\n", arr->data[0]);
  destroy_dynamic_array(&arr);
  printf("Array pointer after destroy: %p\n", (void *)arr);
}
