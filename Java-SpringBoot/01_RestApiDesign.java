/* ============================================
 * NOOB VERSION - Poor REST API Design
 * ============================================ */

@RestController
@RequestMapping("/users") // Should be "/api/v1/users"
class NoobUserController {

  private UserService userService; // Should use constructor injection

  // PROBLEM 1: No proper HTTP status codes
  @GetMapping public List<User> getAllUsers() { // Returns 200 even if empty
    return userService.findAll();
  }

  // PROBLEM 2: Exposing entities directly (not DTOs)
  @GetMapping("/{id}")
  public User getUser(@PathVariable Long id) { // Exposes internal structure
    return userService.findById(id);           // Throws exception on not found
  }

  // PROBLEM 3: Not using ResponseEntity
  @PostMapping public User createUser(@RequestBody User user) { // No validation
    return userService.save(user); // Returns 200 instead of 201
  }

  // PROBLEM 4: Generic exception handling
  @PutMapping("/{id}")
  public User updateUser(@PathVariable Long id, @RequestBody User user) {
    return userService.update(id, user); // What if user not found?
  }

  // PROBLEM 5: No versioning, no pagination, poor naming
  @GetMapping("/search")
  public List<User>
  findUsers(@RequestParam String name) { // Could return millions
    return userService.searchByName(name);
  }

  // PROBLEM 6: Using wrong HTTP method
  @GetMapping("/delete/{id}") // Should be DELETE!
  public String deleteUser(@PathVariable Long id) {
    userService.delete(id);
    return "deleted"; // Returns string instead of proper response
  }
}

/* ============================================
 * CRACKED VERSION - Professional REST API
 * ============================================ */

@Slf4j
@RestController
@RequestMapping("/api/v1/users") // Versioning in URL
@RequiredArgsConstructor // Lombok: generates constructor for final fields
class CrackedUserController {

  private final UserService userService; // Constructor injection (immutable)

  /**
   * Get all users with pagination and sorting
   *
   * @param pageable Pagination parameters (page, size, sort)
   * @return Page of users with metadata
   */
  @GetMapping
  public ResponseEntity<Page<UserResponseDTO>> getAllUsers(Pageable pageable) {
    log.info("Fetching users with pagination: {}", pageable);
    Page<UserResponseDTO> users = userService.findAll(pageable);

    // Return 200 OK with paginated data
    return ResponseEntity.ok(users);
  }

  /**
   * Get user by ID
   *
   * @param id User ID
   * @return User details or 404 if not found
   */
  @GetMapping("/{id}")
  public ResponseEntity<UserResponseDTO> getUserById(@PathVariable Long id) {
    log.info("Fetching user with id: {}", id);

    return userService.findById(id)
        .map(ResponseEntity::ok)                    // 200 OK if found
        .orElse(ResponseEntity.notFound().build()); // 404 if not found
  }

  /**
   * Create new user
   *
   * @param request User creation request with validation
   * @return Created user with 201 status and Location header
   */
  @PostMapping
  @ResponseStatus(HttpStatus.CREATED) // Returns 201 Created
  public ResponseEntity<UserResponseDTO>
  createUser(@Valid @RequestBody UserCreateRequestDTO request) {

    log.info("Creating new user: {}", request.getEmail());
    UserResponseDTO created = userService.create(request);

    // Build Location header with URI of created resource
    URI location = ServletUriComponentsBuilder.fromCurrentRequest()
                       .path("/{id}")
                       .buildAndExpand(created.getId())
                       .toUri();

    // Return 201 Created with Location header
    return ResponseEntity.created(location).body(created);
  }

  /**
   * Update existing user
   *
   * @param id User ID
   * @param request Update request with validation
   * @return Updated user or 404 if not found
   */
  @PutMapping("/{id}")
  public ResponseEntity<UserResponseDTO>
  updateUser(@PathVariable Long id,
             @Valid @RequestBody UserUpdateRequestDTO request) {

    log.info("Updating user with id: {}", id);

    return userService.update(id, request)
        .map(ResponseEntity::ok)                    // 200 OK
        .orElse(ResponseEntity.notFound().build()); // 404 Not Found
  }

  /**
   * Partially update user (PATCH)
   *
   * @param id User ID
   * @param request Partial update request
   * @return Updated user
   */
  @PatchMapping("/{id}")
  public ResponseEntity<UserResponseDTO>
  partialUpdateUser(@PathVariable Long id,
                    @RequestBody UserPatchRequestDTO request) {

    log.info("Partially updating user with id: {}", id);

    return userService.partialUpdate(id, request)
        .map(ResponseEntity::ok)
        .orElse(ResponseEntity.notFound().build());
  }

  /**
   * Delete user
   *
   * @param id User ID
   * @return 204 No Content if successful, 404 if not found
   */
  @DeleteMapping("/{id}")
  public ResponseEntity<Void> deleteUser(@PathVariable Long id) {
    log.info("Deleting user with id: {}", id);

    boolean deleted = userService.deleteById(id);

    return deleted ? ResponseEntity.noContent().build() // 204 No Content
                   : ResponseEntity.notFound().build(); // 404 Not Found
  }

  /**
   * Search users with pagination
   *
   * @param query Search query
   * @param pageable Pagination parameters
   * @return Paginated search results
   */
  @GetMapping("/search")
  public ResponseEntity<Page<UserResponseDTO>>
  searchUsers(@RequestParam(required = false) String query, Pageable pageable) {

    log.info("Searching users with query: '{}', page: {}", query, pageable);

    if (query == null || query.isBlank()) {
      return ResponseEntity.badRequest().build(); // 400 Bad Request
    }

    Page<UserResponseDTO> results = userService.search(query, pageable);
    return ResponseEntity.ok(results);
  }

  /**
   * Get users by status with filtering
   *
   * @param status User status filter
   * @param pageable Pagination
   * @return Filtered users
   */
  @GetMapping("/filter")
  public ResponseEntity<Page<UserResponseDTO>>
  filterUsers(@RequestParam(required = false) String status,
              @RequestParam(required = false) String role, Pageable pageable) {

    log.info("Filtering users - status: {}, role: {}", status, role);

    Page<UserResponseDTO> filtered = userService.filter(status, role, pageable);
    return ResponseEntity.ok(filtered);
  }
}

/* ============================================
 * GLOBAL EXCEPTION HANDLER (Separate Class)
 * ============================================ */

@Slf4j
@RestControllerAdvice
class GlobalExceptionHandler {

  /**
   * Handle validation errors (400 Bad Request)
   */
  @ExceptionHandler(MethodArgumentNotValidException.class)
  public ResponseEntity<ErrorResponse>
  handleValidationException(MethodArgumentNotValidException ex) {

    log.warn("Validation error: {}", ex.getMessage());

    Map<String, String> errors = new HashMap<>();
    ex.getBindingResult().getFieldErrors().forEach(
        error -> errors.put(error.getField(), error.getDefaultMessage()));

    ErrorResponse response = ErrorResponse.builder()
                                 .status(HttpStatus.BAD_REQUEST.value())
                                 .message("Validation failed")
                                 .errors(errors)
                                 .timestamp(LocalDateTime.now())
                                 .build();

    return ResponseEntity.badRequest().body(response);
  }

  /**
   * Handle resource not found (404)
   */
  @ExceptionHandler(ResourceNotFoundException.class)
  public ResponseEntity<ErrorResponse>
  handleNotFoundException(ResourceNotFoundException ex) {

    log.warn("Resource not found: {}", ex.getMessage());

    ErrorResponse response = ErrorResponse.builder()
                                 .status(HttpStatus.NOT_FOUND.value())
                                 .message(ex.getMessage())
                                 .timestamp(LocalDateTime.now())
                                 .build();

    return ResponseEntity.status(HttpStatus.NOT_FOUND).body(response);
  }

  /**
   * Handle all other exceptions (500 Internal Server Error)
   */
  @ExceptionHandler(Exception.class)
  public ResponseEntity<ErrorResponse> handleGenericException(Exception ex) {
    log.error("Unexpected error occurred", ex);

    ErrorResponse response =
        ErrorResponse.builder()
            .status(HttpStatus.INTERNAL_SERVER_ERROR.value())
            .message("An unexpected error occurred")
            .timestamp(LocalDateTime.now())
            .build();

    return ResponseEntity.status(HttpStatus.INTERNAL_SERVER_ERROR)
        .body(response);
  }
}
