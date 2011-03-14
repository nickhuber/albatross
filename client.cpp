#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#ifdef _WIN32
#include <winsock2.h>
#define close closesocket
#endif

#include <stdint.h>
#include <QString>

// debugging
#include <errno.h>
#include <string.h>

#include "client.h"
#include "chatmsg.h"

Client::Client(in_addr_t ip, uint16_t port, const QString& username)
    : socket_(0), running_(true), username_(username) {
    sockaddr_in serverAddress;

    if ((socket_ = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        QString exception("error setting up socket: ");
        exception.append(strerror(errno));
        throw exception;
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = ip;
    serverAddress.sin_port = port;

    if (::connect(socket_, (sockaddr*) &serverAddress, sizeof(serverAddress)) == -1) {
        QString exception("error connecting to server: ");
        exception.append(strerror(errno));
        throw exception;
    }

    QThread::start();
}

Client::~Client() {
    close(socket_);
    running_ = false;

    // wait for the thread to finish.
    wait();
}

void Client::sendMsg(MsgType type, const int length, const char* data) const {
    ChatMsg chatMsg;
    chatMsg.size = length;
    chatMsg.nameSize = username_.size() + 1;
    chatMsg.username = username_.toStdString().c_str();
    chatMsg.type = type;
    chatMsg.data = data;
    // the chatmsg.cpp sendMsg. TODO: move to a namespace
    ::sendMsg(socket_, chatMsg);
    emit signal_messageReceived(username_, chatMsg.data);
}

void Client::run() {
    ChatMsg chatMsg;

    while (running_) {
        switch (readMsg(socket_, chatMsg)) {
            case kSuccess:
                emit signal_messageReceived(chatMsg.username, chatMsg.data);
                break;
            case kDisconnect:
                close(socket_);
                running_ = false;
                break;
            case kError:
                break;
        }
    }
}
