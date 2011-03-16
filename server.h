#ifndef SERVER_H
#define SERVER_H

#include <stdint.h>
#include <QThread>
#include <QString>


/**
  The server waits for new connections, and echos them to every connected client
  (except the one who sent the message). It is scalable to accept many different
  types of messages.

  @author Nick Huber.
  */
class Server : public QThread {
    Q_OBJECT

public:
    /**
      Server constructor. Creates a socket based on the port listening to any
      connections.

      @throws QString
      @param port The port (in network byte order)
      @author Nick huber
      */
    explicit Server(uint16_t port);

    /**
      Destructor for the server. Closes the socket and ends the thread.
      @author Nick Huber
      */
    ~Server();

private:
    int socket_;    /**< The socket to listen on. */
    uint16_t port_; /**< The port to bind to. */
    int backlog_;   /**< The number of backlog connections to maintain. */
    bool running_;  /**< Whether or not the server is currently running. */

protected:
    /**
      Thread routine. Using select() it waits for new clients, and reads data
      from currently connecting clients.

      @author Nick Huber
      */
    void run();

signals:
    /**
      Signal for when a client connects.

      @param address The IP Address of the connected client.
      @author Nick Huber
      */
    void signal_clientConnected(const QString& address);

    /**
      Signal for when a client disconnects.

      @param address The IP Address of the disconnected client.
      @author Nick Huber
      */
    void signal_clientDisconnect(const QString& address);

};

#endif // SERVER_H
