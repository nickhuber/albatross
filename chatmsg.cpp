#ifndef _WIN32
#include <arpa/inet.h>
#endif

#ifdef _WIN32
#include <winsock2.h>
#endif
// debugging
#include <QDebug>
#include <errno.h>
#include <string.h>

#include "chatmsg.h"

return_readMsg readMsg(int socket, ChatMsg& chatMsg) {
    int bytes_to_read = sizeof(chatMsg.size);
    int n;
    char* buffer = new char[bytes_to_read];
    char* buffer_head = buffer;

    // read the message size

    while ((n = recv(socket, buffer, bytes_to_read, 0)) > 0) {
        buffer += n;
        bytes_to_read -= n;
    }

    if (n == -1) {
        qDebug() << "error reading:" << strerror(errno);
        return kError;
    }

    // extra check on first read, if we read nothing then it was a disconnect
    if (n == 0 && bytes_to_read != 0) {
        return kDisconnect;
    }

    chatMsg.size = (int) *buffer_head;
    delete[] buffer_head;


    // read the nameSize

    bytes_to_read = sizeof(chatMsg.nameSize);
    buffer = new char[bytes_to_read];
    buffer_head = buffer;

    while ((n = recv(socket, buffer, bytes_to_read, 0)) > 0) {
        buffer += n;
        bytes_to_read -= n;
    }

    if (n == -1) {
        qDebug() << "error reading:" << strerror(errno);
        return kError;
    }

    chatMsg.nameSize = (int) *buffer_head;
    delete[] buffer_head;

    // read the username

    bytes_to_read = chatMsg.nameSize;
    buffer = new char[bytes_to_read];
    buffer_head = buffer;

    while ((n = recv(socket, buffer, bytes_to_read, 0)) > 0) {
     buffer += n;
     bytes_to_read -= n;
    }

    if (n == -1) {
     qDebug() << "error reading:" << strerror(errno);
     return kError;
    }

    chatMsg.username = buffer_head;

    // read the message type

    bytes_to_read = sizeof(chatMsg.type);
    buffer = new char[bytes_to_read];
    buffer_head = buffer;

    while ((n = recv(socket, buffer, bytes_to_read, 0)) > 0) {
        buffer += n;
        bytes_to_read -= n;
    }

    if (n == -1) {
        qDebug() << "error reading:" << strerror(errno);
        return kError;
    }

    chatMsg.type = (MsgType) *buffer_head;
    delete[] buffer_head;

    // read the message data

    bytes_to_read = chatMsg.size;
    buffer = new char[bytes_to_read];
    buffer_head = buffer;

    while ((n = recv(socket, buffer, bytes_to_read, 0)) > 0) {
        buffer += n;
        bytes_to_read -= n;
    }

    if (n == -1) {
        qDebug() << "error reading:" << strerror(errno);
        return kError;
    }

    chatMsg.data = buffer_head;

    return kSuccess;
}

bool sendMsg(int socket, const ChatMsg& chatMsg) {
    if (send(socket, (void*) &chatMsg.size, sizeof(chatMsg.size), 0) == -1) {
        qDebug() << "error sending:" << strerror(errno);
        return false;
    }
    if (send(socket, (void*) &chatMsg.nameSize, sizeof(chatMsg.nameSize), 0) == -1) {
        qDebug() << "error sending:" << strerror(errno);
        return false;
    }
    if (send(socket, (void*) chatMsg.username, chatMsg.nameSize, 0) == -1) {
        qDebug() << "error sending:" << strerror(errno);
        return false;
    }
    if (send(socket, (void*) &chatMsg.type, sizeof(chatMsg.type), 0) == -1) {
        qDebug() << "error sending:" << strerror(errno);
        return false;
    }
    if (send(socket, (void*) chatMsg.data, chatMsg.size, 0) == -1) {
        qDebug() << "error sending:" << strerror(errno);
        return false;
    }
    return true;
}
