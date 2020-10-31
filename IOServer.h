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

private:
    bool json;

public:
    IOServer(int port, bool json);

    enum Level { ERROR, VERBOSE, DEBUG, INFO };

    long print(const char *message, Level level);

    long print(const char *message);

    sys_ppu_thread_t init(bool await);

    int remote;
    int sock;
private:
    int createSocket(int port);

    static void start(void * arg);
};

#endif //IOSERVER_IOServer_H
