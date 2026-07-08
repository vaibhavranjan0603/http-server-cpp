#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <filesystem>
#include <thread>
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

string readFile(const string& filePath)
{
    ifstream file(filePath, ios::binary);

    if (!file.is_open())
        return "";

    stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

string getContentType(const string& filePath)
{
    if (filePath.size() >= 5 &&
        filePath.substr(filePath.size() - 5) == ".html")
        return "text/html";

    if (filePath.size() >= 4 &&
        filePath.substr(filePath.size() - 4) == ".css")
        return "text/css";

    if (filePath.size() >= 3 &&
        filePath.substr(filePath.size() - 3) == ".js")
        return "application/javascript";

    if (filePath.size() >= 4 &&
        filePath.substr(filePath.size() - 4) == ".png")
        return "image/png";

    if (filePath.size() >= 4 &&
        filePath.substr(filePath.size() - 4) == ".jpg")
        return "image/jpeg";

    if (filePath.size() >= 5 &&
        filePath.substr(filePath.size() - 5) == ".jpeg")
        return "image/jpeg";

    if (filePath.size() >= 4 &&
        filePath.substr(filePath.size() - 4) == ".ico")
        return "image/x-icon";

    return "text/plain";
}

string routeRequest(const string& method,
                    const string& path)
{
    if (method != "GET")
    {
        cout << "[405] " << method << " " << path << endl;

        return buildResponse(
            "405 Method Not Allowed",
            "text/plain",
            "405 Method Not Allowed"
        );
    }

    if (path.find("..") != string::npos)
    {
        cout << "[403] " << path << endl;

        return buildResponse(
            "403 Forbidden",
            "text/plain",
            "403 Forbidden"
        );
    }

    string filePath;

    if (path == "/")
        filePath = "public/index.html";
    else
        filePath = "public" + path;

    if (!filesystem::exists(filePath))
    {
        cout << "[404] " << path << endl;

        return buildResponse(
            "404 Not Found",
            "text/html",
            readFile("public/404.html")
        );
    }

    cout << "[200] " << path << endl;

    return buildResponse(
        "200 OK",
        getContentType(filePath),
        readFile(filePath)
    );
}

void handleClient(int clientFd)
{
    char buffer[4096] = {0};

    int bytesReceived =
        recv(clientFd,
             buffer,
             sizeof(buffer) - 1,
             0);

    if (bytesReceived <= 0)
    {
        close(clientFd);
        return;
    }

    stringstream request(buffer);

    string method;
    string path;
    string version;

    request >> method
            >> path
            >> version;

    cout << "\n---------------------------------\n";
    cout << method << " "
         << path << " "
         << version << endl;

    string response =
        routeRequest(method,
                     path);

    send(clientFd,
         response.c_str(),
         response.size(),
         0);

    close(clientFd);
}

int main()
{
    int serverFd =
        socket(AF_INET,
               SOCK_STREAM,
               0);

    if (serverFd < 0)
    {
        cerr << "Socket creation failed!\n";
        return 1;
    }

    int opt = 1;

    setsockopt(serverFd,
               SOL_SOCKET,
               SO_REUSEADDR,
               &opt,
               sizeof(opt));

    sockaddr_in serverAddr{};

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);

    if (::bind(serverFd,
               (sockaddr*)&serverAddr,
               sizeof(serverAddr)) < 0)
    {
        cerr << "Bind failed!\n";
        return 1;
    }

    if (listen(serverFd, 10) < 0)
    {
        cerr << "Listen failed!\n";
        return 1;
    }

    cout << "=====================================\n";
    cout << " C++ HTTP Server Running\n";
    cout << " http://localhost:8080\n";
    cout << "=====================================\n";

    while (true)
    {
        sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);

        int clientFd =
            accept(serverFd,
                   (sockaddr*)&clientAddr,
                   &clientLen);

        if (clientFd < 0)
            continue;

        thread clientThread(handleClient, clientFd);

        clientThread.detach();
    }

    close(serverFd);

    return 0;
}