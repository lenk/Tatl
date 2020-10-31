//
// Created by air on 10/30/20.
//

#ifndef IOSERVER_IOSERVER_H
#define IOSERVER_IOSERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <iostream>
#include <sys/thread.h>
#include <ppu-types.h>

class IOServer {

protected:
    int remote, sock;
    bool json;

public:
    enum Level { ERROR, VERBOSE, DEBUG, INFO };

    IOServer(int port, bool json);

    sys_ppu_thread_t init(bool await);

    long print(const char *message, Level level);

    long print(const char *message);

private:
    void append(std::stringstream * builder, IOServer::Level level);

    int createSocket(int port);

     static void start(void * arg);
};

#endif //IOSERVER_IOServer_H
