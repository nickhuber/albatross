#ifndef SERVER_H
#define SERVER_H

#include <stdint.h>
#include <QThread>

class Server : public QThread {

private:
    int socket_;
    uint16_t port_;
    int backlog_;

public:
    explicit Server(uint16_t port);
    ~Server();

protected:
    void run();

private:
    void startServer();
};

#endif // SERVER_H
