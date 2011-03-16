#ifndef SERVER_H
#define SERVER_H

#include <stdint.h>
#include <QThread>
#include <QString>

class Server : public QThread {
    Q_OBJECT

private:
    int socket_;
    uint16_t port_;
    int backlog_;
    bool running_;

public:
    explicit Server(uint16_t port);
    ~Server();

protected:
    void run();

signals:
    void signal_clientConnected(const QString& address);
    void signal_clientDisconnect(const QString& address);

};

#endif // SERVER_H
