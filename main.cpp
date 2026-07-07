#include <iostream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

using namespace std;

string buildResponse(const string& status,
                     const string& contentType,
                     const string& body)
{
    return "HTTP/1.1 " + status + "\r\n"
           "Content-Type: " + contentType + "\r\n"
           "Content-Length: " + to_string(body.size()) + "\r\n"
           "Connection: close\r\n"
           "\r\n"
           + body;
}

int main() {

    // Create TCP socket
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);

    if (serverFd < 0) {
        cerr << "Socket creation failed!" << endl;
        return 1;
    }

    // Allow immediate reuse of port
    int opt = 1;
    setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Configure server address
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);

    // Bind socket
    if (::bind(serverFd, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Bind failed!" << endl;
        close(serverFd);
        return 1;
    }

    // Start listening
    if (listen(serverFd, 5) < 0) {
        cerr << "Listen failed!" << endl;
        close(serverFd);
        return 1;
    }

    cout << "Server listening on http://localhost:8080" << endl;

    while (true) {

        sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);

        int clientFd = accept(serverFd,
                              (sockaddr*)&clientAddr,
                              &clientLen);

        if (clientFd < 0) {
            cerr << "Accept failed!" << endl;
            continue;
        }

        cout << "Client connected!" << endl;

        char buffer[4096] = {0};

        int bytesReceived = recv(clientFd,
                                 buffer,
                                 sizeof(buffer) - 1,
                                 0);

        if (bytesReceived < 0) {
            cerr << "recv failed!" << endl;
            close(clientFd);
            continue;
        }

        if (bytesReceived == 0) {
            cout << "Client disconnected without sending data." << endl;
            close(clientFd);
            continue;
        }

        cout << "----- Raw request received -----" << endl;
        cout.write(buffer, bytesReceived);
        cout << endl;
        cout << "--------------------------------" << endl;

        stringstream requestStream(buffer);

        string method;
        string path;
        string version;

        requestStream >> method >> path >> version;

        cout << endl;
        cout << "Method  : " << method << endl;
        cout << "Path    : " << path << endl;
        cout << "Version : " << version << endl;
        cout << endl;

        string body = "Hello from my C++ HTTP Server!";

        string response = buildResponse(
            "200 OK",
            "text/plain",
            body
        );

        ssize_t bytesSent = send(clientFd,
                                 response.c_str(),
                                 response.size(),
                                 0);

        if (bytesSent < 0) {
            cerr << "send failed!" << endl;
        }

        close(clientFd);
    }

    close(serverFd);

    return 0;
}