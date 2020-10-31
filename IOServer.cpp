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

/**
 * create socket connection with selected port to listen for incoming connection
 *
 * @param port port to listen on
 * @return socket ptr
 */
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

/**
 * call to initialize server and accept any incoming connection
 * set 'await' as true to block your thread till connection is accepted
 *
 * @param await wait for accepted connection
 * @return running thread
 */
sys_ppu_thread_t IOServer::init(bool await) {
    sys_ppu_thread_t t;

    sysThreadCreate(&t, start, this, 0, 0x2000, 1, "IOServer");

    if(await) {
        sysThreadJoin(t, 0);
    }

    return t;
}

/**
 * accept first incoming connection and store remote socket connection
 * to communicate all prints through
 *
 * @param ptr IOServer pointer
 */
void IOServer::start(void * ptr) {
    IOServer * server = static_cast<IOServer *>(ptr);

    if (listen(server->sock, 5) < 0) {
        std::cout << "error listening for connection" << std::endl;
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in remoteAddress;
    if ((server->remote = accept(server->sock, (struct sockaddr *) &remoteAddress,
                                 reinterpret_cast<socklen_t *>(sizeof(remoteAddress)))) < 0) {

        std::cout << "error accepting connection" << std::endl;
        exit(EXIT_FAILURE);
    }
}

/**
 * print raw data/message to remote client
 *
 * @param message data to send
 * @return send socket response
 */
long IOServer::print(const char *message) {
    return send(this->remote, message, strlen(message), 0);
}

/**
 * print message with message type, if JSON is toggled
 * message is sent as a JSON built message, otherwise
 * it's sent in the following format
 * (LEVEL): (MESSAGE)
 *
 * @param message message to send
 * @param level selected message level
 * @return send socket response
 */
long IOServer::print(const char *message, Level level) {
    std::stringstream builder;

    if(this->json) {
        builder << "{\"type\":\"";
        append(&builder, level);
        builder << "\", \"message\":\"" << message << "\"}";
    } else {
        append(&builder, level);
        builder << ": " << message;
    }

    const std::string tmp = builder.str();
    const char* result = tmp.c_str();

    return send(this->remote, result, strlen(result), 0);
}

/**
 * append selected enum level to StringStream as a readable string
 *
 * @param builder stream
 * @param level selected level
 */
void IOServer::append(std::stringstream * builder, IOServer::Level level) {
    switch (level) {
        case IOServer::Level::DEBUG:
            builder->operator<<("DEBUG");
            break;

        case IOServer::Level::VERBOSE:
            builder->operator<<("VERBOSE");
            break;

        case IOServer::Level::ERROR:
            builder->operator<<("ERROR");
            break;

        case IOServer::Level::INFO:
            builder->operator<<("INFO");
            break;
    }
}