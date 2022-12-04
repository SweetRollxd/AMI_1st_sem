#ifndef WEBSERVER_H
#define WEBSERVER_H

#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <map>
#include <fstream>

const int QUEUE_SIZE = 5;
const int BUF_SIZE = 1024;

typedef std::map<std::string, std::string> http_request;

class WebServer
{
//    int port;
    int listenSocket;
    sockaddr_in address;

    void handleRequest(int client_socket);
    http_request parseRequest(std::string request);
    std::string recvAll(int client_socket);
    int sendAll(int client_socket, std::string message);

public:
    WebServer(int port);
    int run();
    ~WebServer();
};

#endif // WEBSERVER_H
