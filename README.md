# ssl-app

## Description

**ssl-app** is a full-stack application built entirely in C++ with integrated support for HTML, JavaScript, and CSS. This application is designed to handle HTTP requests and responses, including secure SSL/TLS connections, using Boost Beast and Boost Asio. It features a backend with Redis integration for handling user authentication and session management.

## Dependencies

To compile and run this application, ensure the following libraries and tools are installed:

- **g++** (C++17 or later)
- **Boost Libraries**:
  - **Boost Asio** (for asynchronous networking)
  - **Boost Beast** (for HTTP and WebSocket handling)
  - **Boost System** (for system error handling)
  - **Boost Filesystem** (for filesystem manipulation)
  - **Boost Thread** (for multithreading)
  - **Boost JSON** (for JSON parsing and serialization)
- **OpenSSL** (for SSL/TLS support)
- **Redis++** (for Redis integration in C++)
- **Hiredis** (Redis client library)

## Features

### Redis Service
The `RedisService` class is responsible for interacting with the Redis database. It handles operations such as storing and retrieving user data, including credentials, and validates user logins by checking the credentials against the stored data.

### Client Service
The `ClientService` class manages HTTP requests to external services, allowing the application to act as an HTTP client. It can send GET requests to specified hosts and ports, retrieve responses, and manage connections.

## Running the Application

To run the application, use the following command:

```bash
./main 0.0.0.0 8080 www 2
```

- 0.0.0.0: The address on which the server listens (binds to all interfaces).
- 8080: The port on which the server listens for incoming connections.
- www: The root directory for serving static files.
- 2: The number of threads to use for handling requests.

### Compiling

To compile the application, use the following command:

```bash
g++ -std=c++17 main.cpp src/http_tools.cpp src/listener.cpp src/session.cpp src/application.cpp src/services.cpp -o main -lboost_system -lboost_filesystem -lboost_thread -lssl -lcrypto -lpthread -lhiredis -lredis++
```

This command compiles all the source files into a single executable named main, linking the necessary libraries for Boost, OpenSSL, and Redis++.

### Usage

After compiling, simply run the application with the provided command. The server will start and begin accepting HTTP requests on the specified address and port, with integrated SSL support and Redis-based authentication.
