#include "server.h"
#include <QDebug>

Server::Server(int port) : port_(port) {
    startServer();
}

void Server::startServer() {
    qDebug() << "chump";
}

