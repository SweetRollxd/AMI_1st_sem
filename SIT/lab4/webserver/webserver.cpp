#include "webserver.h"

void WebServer::handleRequest(int client_socket)
{
    std::cout << "New thread started!" << std::endl;
    std::string msg = recvAll(client_socket);
    std::cout << "Response: " << msg << std::endl;

    http_request req_data = parseRequest(msg);
    std::cout << req_data["method"] << " " << req_data["path"] << " " << req_data["version"] << std::endl;

    std::string response_body;
    std::ifstream fp;
    std::string buf;

    fp.open("../index.html");
    while (getline (fp, buf)) response_body.append(buf);

    std::string response = "HTTP/1.1 200 OK\r\n";
    response.append("Version: HTTP/1.1\r\n");
    response.append("Content-Type: text/html; charset=utf-8\r\n");
    response.append("Content-length: " + std::to_string(response_body.length()));
    response.append("\r\n\r\n");
    response.append(response_body);
//      <<
//      <<
//      << "" << response_body.str().length()
//      << "\r\n\r\n"


    std::cout << "Reponse: " << response << std::endl;
    int n = sendAll(client_socket, response);
    if (n < 0){
        perror("sending");
        close(client_socket);
    }
    std::cout << "Sent " << n << " bytes" << std::endl;
    close(client_socket);
    std::cout << "Socket closed, returning..." << std::endl;

    return;
}

http_request WebServer::parseRequest(std::string request)
{
    auto head = request.begin();
    auto end = request.begin();
    http_request req_data;

    while (*end !=  ' ') end++;
    req_data["method"] = std::string(head, end);

    end++;
    head = end;
    while (*end != ' ') end++;
    req_data["path"] = std::string(head, end);

    end++;
    head = end;
    while (*end !=  ' ') end++;
    req_data["version"] = std::string(head, end);

    return req_data;
}

std::string WebServer::recvAll(int client_socket)
{

    char buf[BUF_SIZE];
    int n = BUF_SIZE;
    std::string message = "";
    while (n == BUF_SIZE) {
        n = recv(client_socket, buf, BUF_SIZE, 0);
        if (n == 0) throw("Connection closed");
        message.append(buf);
    }
    return message;
}

int WebServer::sendAll(int client_socket, std::string message)
{
    std::cout << "Send msg" << std::endl;
    const char* buf = message.c_str();
//    message.copy(buf, message.length(), 0);
//    buf[message.length()] = '\0';
    std::cout << "Message copied to C-string" << std::endl;

//    int n = BUF_SIZE;
    int total = 0;
    int bytes_sent = 0;
    while (total < message.length()) {
        bytes_sent = send(client_socket, buf + total, message.length(), 0);
        if (bytes_sent == -1) break; //throw("Connection closed");
        total += bytes_sent;
    }
    return (bytes_sent == -1 ? -1 : total);
}

WebServer::WebServer(int port)
{
    this->listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0){
        perror("socket");
        exit(1);
    }

    this->address.sin_addr.s_addr = htonl(INADDR_ANY);
    this->address.sin_port = htons(port);
    this->address.sin_family = AF_INET;

    if (bind(listenSocket, (sockaddr *) &address, sizeof(address)) < 0) {
        perror("bind");
        close(listenSocket);
        exit(1);
    }
}

int WebServer::run()
{
    if (listen(listenSocket, QUEUE_SIZE)) {
        perror("listen");
        close(listenSocket);
        exit(1);
    }

    sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int accept_sock;
    while(1){
        if((accept_sock = accept(listenSocket, (sockaddr *) &client_addr, &client_addr_len)) < 0){
            perror("accept");
            close(listenSocket);
            exit(1);
        }

        std::cout << "Starting new thread..." << std::endl;
        try {
            // Создаем новый поток для сокета клиента, отправившего запрос
            std::thread(&WebServer::handleRequest, this, accept_sock).detach();
        }
        catch(std::exception e){
            std::cout << "Errno: " << errno << "Exception: " << e.what();
        }
    }
}

WebServer::~WebServer()
{

}
