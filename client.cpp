#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#ifdef _WIN32
#include <winsock2.h>
#define close closesocket
typedef int socklen_t;
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
    memset((void*) &chatMsg.address, 0, sizeof(chatMsg.address));
    chatMsg.username = username_.toStdString().c_str();
    chatMsg.type = type;
    chatMsg.data = data;
    // the chatmsg.cpp sendMsg. TODO: move to a namespace
    ::sendMsg(socket_, chatMsg);
    emit signal_messageReceived(username_, "self", chatMsg.data);
}

void Client::run() {
    ChatMsg chatMsg;

    while (running_) {
        switch (readMsg(socket_, chatMsg)) {
            case kSuccess:
                emit signal_messageReceived(chatMsg.username, QString(inet_ntoa(chatMsg.address)), chatMsg.data);
                break;
            case kDisconnect:
                close(socket_);
                running_ = false;
                emit signal_disconnected();
                break;
            case kError:
                break;
        }
    }
}
