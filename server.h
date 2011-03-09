#ifndef SERVER_H
#define SERVER_H

#include <stdint.h>

class Server {

private:
    int socket_;
    uint16_t port_;
    int backlog_;

public:
    Server(uint16_t port);
    ~Server();

private:
    void startServer();
    void serving();
};

#endif // SERVER_H
