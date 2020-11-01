#include <iostream>

#include "sys/socket.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <cstring>
#include <sstream>

class IOServer {
private:
    struct sockaddr_in addr;
    int _connection;
    int _socket;
    bool _json;

public:
    enum Level { ERROR, VERBOSE, DEBUG, INFO };

    IOServer(int port, bool json) {
        this->_json = json;
        this->initSocket(port);
    }

    /**
     * create socket connection with selected port to listen for incoming connection
     *
     * @param port port to listen on
     * @return socket ptr
     */
private:
    void initSocket(int port) {
        if ((_connection = sysNetSocket(AF_INET, SOCK_STREAM, 0)) == 0) {
            throw std::runtime_error("failure creating socket stream!");
        }

        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(port);
        addr.sin_family = AF_INET;

        if (sysNetBind(_connection, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
            throw std::runtime_error("failure binding connection!");
        }

        if (sysNetListen(_connection, 3) < 0) {
            throw std::runtime_error("failure listening connection!");
        }
    }

    /**
     * call to initialize server and accept any incoming connection
     * set 'await' as true to block your thread till connection is accepted
     *
     * @param await wait for accepted connection
     * @return running thread
     */
public:
    void init(bool await) {
        start();
    }

    /**
     * accept first incoming connection and store remote socket connection
     * to communicate all prints through
     *
     * @param ptr IOServer pointer
     */
    void start() {
        int len = sizeof(addr);

        if ((_socket = sysNetAccept(_connection, (struct sockaddr *) &addr, (socklen_t * ) &len)) < 0) {
            throw std::runtime_error("failure accepting connection!");
        }
    }

    /**
     * print raw data/message to remote client
     *
     * @param message data to send
     * @return send socket response
     */
    void print(const char *message) {
        sysNetSendto(this->_socket, message, strlen(message), 13, (struct sockaddr *) &addr, sizeof(addr));
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
    void print(const char *message, Level level) {
        std::stringstream builder;

        if (this->_json) {
            builder << "{\"type\":\"";
            append(&builder, level);
            builder << "\", \"message\":\"" << message << "\"}\n\n";
        } else {
            append(&builder, level);
            builder << ": " << message;
        }

        const std::string tmp = (builder.str());
        const char *result = tmp.c_str();

        print(result);
    }

    /**
     * append selected enum level to StringStream as a readable string
     *
     * @param builder stream
     * @param level selected level
     */
    void append(std::stringstream *builder, Level level) {
        switch (level) {
            case Level::DEBUG:
                builder->operator<<("DEBUG");
                break;

            case Level::VERBOSE:
                builder->operator<<("VERBOSE");
                break;

            case Level::ERROR:
                builder->operator<<("ERROR");
                break;

            case Level::INFO:
                builder->operator<<("INFO");
                break;
        }
    }
};
