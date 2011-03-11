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

    if (::connect(socket_, (sockaddr*) &serverAddress, sizeof(serverAddress)) == -1) {
        qDebug() << "error conencting to server:" << strerror(errno);
        throw "error";
    }

    QThread::start();
}

Client::~Client() {
    close(socket_);
    running_ = false;
    // TODO: convert this to a mutex
    while(isRunning()) { // wait for thread to exit
        ;
    }
}

void Client::sendTextMsg(const QString& msg) {
    ChatMsg chatMsg;
    chatMsg.size = msg.size() + 1;
    chatMsg.data = msg.toStdString().c_str();
    chatMsg.type = kChat;
    sendMsg(socket_, chatMsg);
}

void Client::run() {
    ChatMsg chatMsg;
    while (running_) {
        switch (readMsg(socket_, chatMsg)) {
        case kSuccess:
            qDebug() << chatMsg.data;
            break;
        case kDisconnect:
            qDebug() << "connection closed";
            close(socket_);
            break;
        case kError:
            break;
        }
    }
}
