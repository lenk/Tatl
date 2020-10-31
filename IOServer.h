//
// Created by air on 10/30/20.
//

#ifndef IOSERVER_IOSERVER_H
#define IOSERVER_IOSERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <iostream>
#include <thread>

class IOServer {

private:
    int remote;
    bool json;
    int sock;

public:
    IOServer(int port, bool json);

    enum Level { ERROR, VERBOSE, DEBUG, INFO };

    long print(const char *message, Level level);

    long print(const char *message);

    void init(bool await);

private:
    int createSocket(int port);

    void start();
};

#endif //IOSERVER_IOServer_H
