//
// Created by air on 10/30/20.
//
#include "IOServer.h"
#include <cstring>
#include <sstream>

IOServer::IOServer(int port, bool json) {
    this->sock = createSocket(port);
    this->json = json;
}

int IOServer::createSocket(int port) {
    int connection = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in name;

    if(connection < 0) {
        std::cout << "error creating socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    name.sin_addr.s_addr = htonl(INADDR_ANY);
    name.sin_port = htons (port);
    name.sin_family = AF_INET;

    if(bind(connection, (struct sockaddr *) &name, sizeof(name)) < 0) {
        std::cout << "error binding socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    return connection;
}

void IOServer::init(bool await) {
    std::thread t(&IOServer::start, this);

    if(await) {
        t.join();
    }
}

void IOServer::start() {
    struct sockaddr_in name;
    socklen_t len = sizeof(name);

    if(listen(this->sock, 5) < 0) {
        std::cout << "error listening for connection" << std::endl;
        exit(EXIT_FAILURE);
    }

    if((this->remote = accept(this->sock, (struct sockaddr *) &name, &len)) < 0) {
        std::cout << "error accepting connection" << std::endl;
        exit(EXIT_FAILURE);
    }
}

long IOServer::print(const char *message) {
    return send(this->remote, message, 16, 0);
}

long IOServer::print(const char *message, Level level) {
    std::stringstream builder;

    if(this->json) {
        builder << "{\"type\":\"";
    }

    switch (level) {
        case DEBUG:
            builder << "DEBUG";
            break;

        case VERBOSE:
            builder << "VERBOSE";
            break;

        case ERROR:
            builder << "ERROR";
            break;

        case INFO:
            builder << "INFO";
            break;
    }

    if(this->json) {
        builder << "\", \"message\":\"" << message << "\"}";
    } else {
        builder << ": " << message;
    }

    const std::string tmp = builder.str();
    const char* result = tmp.c_str();

    return send(this->remote, result, strlen(result), 0);
}