# C++ HTTP Server

A lightweight multithreaded HTTP server built completely from scratch in C++ using POSIX sockets without relying on any web framework.

---

## Features

- TCP server using POSIX socket APIs
- Multithreaded request handling using `std::thread`
- HTTP/1.1 request parsing
- Static file serving (HTML and CSS)
- URL routing
- MIME type detection
- Custom 404 error page
- 403 Forbidden for directory traversal attempts
- 405 Method Not Allowed for unsupported HTTP methods
- Binary-safe file reading
- Request logging

---

## Project Structure

```
http-server-cpp/
│
├── main.cpp
├── public/
│   ├── index.html
│   ├── about.html
│   ├── 404.html
│   └── style.css
│
├── .gitignore
└── README.md
```

---

## How It Works

1. Creates a TCP socket.
2. Binds the socket to port **8080**.
3. Starts listening for incoming connections.
4. Accepts incoming client connections.
5. Creates a separate thread for every client.
6. Receives and parses the HTTP request.
7. Routes the requested URL.
8. Reads the requested file from disk.
9. Builds an HTTP response.
10. Sends the response back to the client.

---

## Supported Routes

| Route | Description |
|-------|-------------|
| `/` | Home page |
| `/about.html` | About page |
| Any invalid path | Custom 404 page |

---

## Security Features

- Prevents directory traversal attacks (`../`)
- Returns proper HTTP status codes
- Restricts unsupported HTTP methods

---

## Technologies Used

- C++17
- POSIX Socket Programming
- std::thread
- std::filesystem
- HTML
- CSS

---

## Build

Compile

```bash
clang++ -std=c++17 -pthread main.cpp -o server
```

Run

```bash
./server
```

Open in your browser

```
http://localhost:8080
```

---

## Sample Server Log

```text
GET / HTTP/1.1
[200] /

GET /about.html HTTP/1.1
[200] /about.html

GET /random HTTP/1.1
[404] /random
```

---

## Future Improvements

- Thread pool implementation
- HTTP Keep-Alive
- POST request handling
- HTTPS / TLS support
- File upload support
- HTTP caching

---

## Author

**Vaibhav Ranjan**

Delhi Technological University (DTU)

Computer Science and Engineering