# REST API Design Best Practices


**Key Concepts:**
- RESTful principles and HTTP methods
- Proper status codes (200, 201, 204, 404, etc.)
- DTOs vs Entities
- Pagination and filtering
- Global exception handling

**Resources:**
- [Spring Boot REST Tutorial](https://spring.io/guides/tutorials/rest/)
- [REST API Best Practices](https://stackoverflow.blog/2020/03/02/best-practices-for-rest-api-design/)
- [Microsoft REST API Guidelines](https://github.com/microsoft/api-guidelines/blob/vNext/Guidelines.md)
- [Richardson Maturity Model](https://martinfowler.com/articles/richardsonMaturityModel.html)
- [Baeldung - REST with Spring](https://www.baeldung.com/rest-with-spring-series)

> **Core Philosophy**: A well-designed REST API is intuitive, consistent, and treats HTTP as an application protocol, not just a transport mechanism.

## Why REST API Design Matters

Poor API design creates technical debt that's difficult to fix once clients depend on it. A well-designed API reduces integration time, prevents bugs, improves security, and makes your service easier to maintain and evolve over time.

---

## URL Design Principles

### Use Nouns, Not Verbs

URLs should represent resources (things), not actions. The HTTP method indicates the action. This keeps your API consistent and predictable—clients know that POST to any collection URL creates a resource, GET retrieves it, and so on.

### Use Plural Nouns

Consistency matters more than grammar. Using plural forms for all collections avoids the confusion of switching between singular and plural. It also makes the pattern clearer: the collection is many items, individual items are accessed by ID within that collection.

### Version Your API

APIs evolve, but existing clients can't break when you make changes. Versioning in the URL path makes it explicit which API version a client uses. This allows you to introduce breaking changes in new versions while maintaining backward compatibility for existing clients.

### Use Hierarchical Structure

Resources often have relationships. Expressing these relationships in the URL hierarchy makes the API self-documenting and intuitive. When you see a URL like `/users/123/orders/456`, you immediately understand the relationship: order 456 belongs to user 123.

---

## HTTP Methods: The Verbs of REST

### GET - Retrieve Resources

GET requests should be safe and idempotent—calling them multiple times produces the same result with no side effects. They should never modify data. This allows caching, bookmarking, and safe retrying of failed requests.

### POST - Create New Resources

POST creates new resources where the server assigns the identifier. It's not idempotent—calling it twice creates two resources. The response should include a Location header pointing to the newly created resource.

### PUT - Replace Entire Resources

PUT replaces a resource completely with the provided representation. It's idempotent—sending the same PUT multiple times results in the same final state. The client must send all fields, even unchanged ones, because PUT is a full replacement operation.

### PATCH - Partial Updates

PATCH modifies only the specified fields of a resource. This is more efficient than PUT when clients only need to change one or two fields—they don't need to send the entire resource representation. However, PATCH semantics can be more complex to implement correctly.

### DELETE - Remove Resources

DELETE removes a resource. Like PUT, it's idempotent—deleting the same resource multiple times results in the same state (the resource is gone). The second and subsequent DELETE requests typically return 404 since the resource no longer exists.

---

## Status Codes: Communicating Results

### Success Codes

**200 OK** indicates successful GET, PUT, or PATCH operations. It typically includes the resource representation in the response body.

**201 Created** signals successful resource creation via POST. Include a Location header with the new resource's URL so clients can immediately access it without parsing response bodies.

**204 No Content** is perfect for successful DELETE operations or updates where there's nothing meaningful to return. It tells the client "your request succeeded, but there's no response body."

### Client Error Codes

**400 Bad Request** indicates the client sent invalid data. Use this for validation failures, malformed JSON, or missing required fields. Include detailed error messages explaining what went wrong.

**404 Not Found** means the requested resource doesn't exist. This applies to both collection items and entire endpoints. Don't use 404 for authorization failures—that leaks information about resource existence.

### Server Error Codes

**500 Internal Server Error** represents unexpected server failures. These should be rare in production—most errors should be anticipated and return appropriate 4xx codes. Always log 500 errors for investigation.

---

## Request and Response Design

### Use DTOs, Not Entities

Never expose database entities directly through your API. DTOs (Data Transfer Objects) decouple your API contract from your internal data model. This allows you to change your database schema without breaking client contracts, and lets you control exactly what data is exposed.

### Validate Input with Annotations

Use validation annotations like @Valid, @NotNull, and @Size to enforce constraints declaratively. This keeps validation logic close to the data model, makes requirements explicit in code, and provides consistent error messages automatically.

### Use ResponseEntity for Flexibility

ResponseEntity gives you complete control over HTTP responses—status codes, headers, and body. This flexibility is essential for proper REST API design where status codes and headers convey important semantic information beyond just the response payload.

### Implement Pagination for Collections

Returning entire collections is inefficient and doesn't scale. Pagination limits response size, improves performance, and prevents timeout issues. Support standard pagination parameters like page, size, and sort to give clients control over the data they receive.

### Support Filtering and Sorting

Clients often need subsets of data matching specific criteria. Query parameters for filtering and sorting make your API more useful without requiring multiple specialized endpoints. Keep filter syntax simple and well-documented.

---

## Error Handling Strategy

### Use Global Exception Handling

@RestControllerAdvice provides centralized exception handling across all controllers. This ensures consistent error response formats, reduces code duplication, and makes it easy to change error handling behavior globally without touching individual controllers.

### Return Consistent Error Responses

Clients need to parse error responses programmatically. A consistent error structure—with fields like timestamp, status, message, and details—makes client-side error handling straightforward. Inconsistent error formats force clients to handle each endpoint differently.

### Include Helpful Error Messages

Vague errors like "invalid request" waste everyone's time. Specific messages like "email field is required" or "price must be positive" help developers fix problems immediately during integration. Balance helpfulness with security—don't leak sensitive implementation details.

### Log Errors Appropriately

Different errors need different logging levels. Client errors (4xx) might only need INFO level—they're expected. Server errors (5xx) require ERROR level with full stack traces for debugging. Include request context like user ID and correlation IDs to trace issues across distributed systems.

---

## Security Fundamentals

### Validate and Sanitize Input

Never trust client input. Validate all data against expected formats and ranges. Sanitize input to prevent injection attacks. This is your first line of defense against malicious requests and prevents bad data from corrupting your system.

### Use HTTPS in Production

HTTPS encrypts data in transit, preventing eavesdropping and man-in-the-middle attacks. It's non-negotiable for any production API, especially those handling authentication credentials or sensitive data. Many security features like secure cookies require HTTPS.

### Implement Authentication and Authorization

Know who's making requests (authentication) and what they're allowed to do (authorization). Use standard protocols like OAuth 2.0 or JWT rather than inventing your own security schemes. Always validate permissions for every request—never trust client-provided authorization data.

### Rate Limiting for Public APIs

Protect your service from abuse and ensure fair resource distribution. Rate limiting prevents a single client from monopolizing resources or launching denial-of-service attacks. It also encourages efficient client implementations that batch requests appropriately.

---
