#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#ifdef _WIN32
#include <winsock2.h>
#endif

#include <stdint.h>

// debugging
#include <QDebug>
#include <errno.h>
#include <string.h>

#include "client.h"
#include "chatmsg.h"

Client::Client(in_addr_t ip, uint16_t port, const QString& username) : socket_(0), running_(true), username_(username) {
    sockaddr_in serverAddress;

    if ((socket_ = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        qDebug() << "error setting up socket:" << strerror(errno);
        throw;
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = ip;
    serverAddress.sin_port = port;

    if (::connect(socket_, (sockaddr*) &serverAddress, sizeof(serverAddress)) == -1) {
        qDebug() << "error conencting to server:" << strerror(errno);
        throw;
    }

    QThread::start();
}

Client::~Client() {
    shutdown(socket_, SHUT_RDWR);
    running_ = false;
    // TODO: convert this to a mutex
    while(isRunning()) { // wait for thread to exit
        ;
    }
}

void Client::sendMsg(MsgType type, const int length, const char* data) const {
    ChatMsg chatMsg;
    chatMsg.size = length;
    chatMsg.nameSize = username_.size() + 1;
    chatMsg.username = username_.toStdString().c_str();
    chatMsg.type = type;
    chatMsg.data = data;
    ::sendMsg(socket_, chatMsg);
    emit signal_displayMessage(username_, chatMsg.data);
}

void Client::run() {
    ChatMsg chatMsg;
    while (running_) {
        switch (readMsg(socket_, chatMsg)) {
        case kSuccess:
            emit signal_displayMessage(chatMsg.username, chatMsg.data);
            break;
        case kDisconnect:
            shutdown(socket_, SHUT_RDWR);
            running_ = false;
            break;
        case kError:
            break;
        }
    }
}
