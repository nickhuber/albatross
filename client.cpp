#include <sys/socket.h>
#include <netinet/in.h>
#include <stdint.h>

// debugging
#include <QDebug>
#include <errno.h>
#include <string.h>

#include "client.h"
#include "chatmsg.h"

Client::Client(in_addr_t ip, uint16_t port, const QString& username) {
    sockaddr_in serverAddress;

    if ((socket_ = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        qDebug() << "error setting up socket:" << strerror(errno);
        throw "error";
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = ip;
    serverAddress.sin_port = port;

    if (connect(socket_, (sockaddr*) &serverAddress, sizeof(serverAddress)) == -1) {
        qDebug() << "error conencting to server:" << strerror(errno);
        throw "error";
    }
}

Client::~Client() {
    close(socket_);
}

void Client::sendMsg(const QString &msg) {
    ChatMsg chatMsg;

    chatMsg.size = msg.size() + 1;
    chatMsg.data = msg.toStdString().c_str();
    chatMsg.type = CHAT;
    if (send(socket_, (void*) &chatMsg.size, sizeof(chatMsg.size), 0) == -1) {
        qDebug() << "error sending:" << strerror(errno);
    }
    if (send(socket_, (void*) &chatMsg.type, sizeof(chatMsg.type), 0) == -1) {
        qDebug() << "error sending:" << strerror(errno);
    }
    if (send(socket_, (void*) chatMsg.data, chatMsg.size, 0) == -1) {
        qDebug() << "error sending:" << strerror(errno);
    }
}
