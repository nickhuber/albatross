#ifndef CLIENT_H
#define CLIENT_H

#ifndef _WIN32
#include <arpa/inet.h>
#include <sys/types.h>
#endif

#ifdef _WIN32
#include <winsock2.h>
#endif

#include <stdint.h>
#include <QString>
#include <QThread>

#include "chatmsg.h"

class Client : public QThread
{
    Q_OBJECT

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

    void sendMsg(MsgType type, const int length, const char* data) const ;
private:
    int socket_;
    bool running_;
    QString username_;

signals:
    void signal_displayMessage(const QString& username, const QString& message) const;

protected:
    void run();
};

#endif // CLIENT_H
