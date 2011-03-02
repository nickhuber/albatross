#ifndef CLIENT_H
#define CLIENT_H

#include <QString>

class Client
{
public:
    Client(uint32_t ip, uint16_t port, const QString& username);
private:
    int socket_;
};

#endif // CLIENT_H
