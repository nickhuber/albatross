#ifndef CLIENT_H
#define CLIENT_H

#include <arpa/inet.h>
#include <sys/types.h>
#include <stdint.h>
#include <QString>

class Client
{
public:
    Client(in_addr_t ip, uint16_t port, const QString& username);
private:
    int socket_;
};

#endif // CLIENT_H
