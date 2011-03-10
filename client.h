#ifndef CLIENT_H
#define CLIENT_H

#include <arpa/inet.h>
#include <sys/types.h>
#include <stdint.h>
#include <QString>

class Client
{
public:
    /**
      Constructor for the client. Opens a connection.

      @param ip The IPv4 address.
      @param port The port number.
      @param username The client's username.
      @author Nick Huber
      */
    Client(in_addr_t ip, uint16_t port, const QString& username);

    /**
      Destructor for the client. Closes the associated socket.
      @author Nick Huber
      */
    ~Client();
private:
    int socket_;
};

#endif // CLIENT_H
