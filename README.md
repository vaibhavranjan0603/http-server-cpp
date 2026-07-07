# HTTP Server in C++

A lightweight HTTP server built completely from scratch in C++ using POSIX sockets.

## Features

- TCP socket programming
- HTTP request parsing
- Parses HTTP method, path and version
- Generates HTTP responses
- No external frameworks

## Build

```bash
clang++ -std=c++17 main.cpp -o server
./server
```

## Test

```bash
curl http://localhost:8080
curl http://localhost:8080/hello
```

## Current Progress

### Part 1
- TCP server
- Request parsing
- Response builder