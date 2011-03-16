/**
  @file

  Everything related to the Client.

  Works cross platform. Although there was a typedef and some include changes
  required to get windows to cooperate.

  */

#ifndef CLIENT_H
#define CLIENT_H

#ifndef _WIN32
#include <arpa/inet.h>
#include <sys/types.h>
#endif

#ifdef _WIN32
typedef unsigned int in_addr_t;
#include <winsock2.h>
#endif

#include <stdint.h>
#include <QString>
#include <QThread>

#include "chatmsg.h"


/**
  The client connects to a server, and through it sends and receives messages
  to other clients connected to he same server.

  @author Nick Huber
  */
class Client : public QThread {
    Q_OBJECT

public:
    /**
      Constructor for the client. Opens a connection.

      @throws QString
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
    int socket_;       /**< Handle to the socket. */
    bool running_;     /**< Whether or not the thread should be running. */
    QString username_; /**< This client's username. */

signals:
    /**
      Signal for the client received a message.

      @param username The username to show.
      @param message The message to show.
      @author Nick Huber
      */
    void signal_messageReceived(const QString& username, const QString& message) const;

    /**
      Signal for the client disconnecting.

      @author Nick Huber
      */
    void signal_disconnected();

protected:
    /**
      The client thread method, receives any incoming data and signals the GUI
      to update when it does.

      @author Nick Huber.
      */
    void run();
};

#endif // CLIENT_H
