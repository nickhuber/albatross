#include <sys/socket.h>
#include <netinet/in.h>
#include <stdint.h>
#include <QDebug>

#include "client.h"

Client::Client(in_addr_t ip, uint16_t port, const QString& username)
{
    sockaddr_in serverAddress;

    if ((socket_ = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        qDebug() << "socket error";
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = ip;
    serverAddress.sin_port = port;

    if (connect(socket_, (sockaddr*) &serverAddress, sizeof(serverAddress)) == -1) {
        qDebug() << "error conencting to server";
    }

}
