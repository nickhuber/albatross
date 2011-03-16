#ifndef _WIN32
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

#ifdef _WIN32
#define close closesocket
#include <winsock2.h>
typedef int socklen_t;
#endif

// debugging
#include <QDebug>
#include <errno.h>
#include <string.h>

#include "server.h"
#include "chatmsg.h"

Server::Server(uint16_t port)
    : socket_(0), port_(port), backlog_(5), running_(true) {

    if ((socket_ = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        QString exception("error creating server socket: ");
        exception.append(strerror(errno));
        throw exception;
    }

    int arg = 1;

    if (setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, (char*) &arg, sizeof(arg)) == -1) {
        QString exception("error setting up socket: ");
        exception.append(strerror(errno));
        throw exception;
    }

    sockaddr_in listenTo;
    memset((char*) &listenTo, 0, sizeof(listenTo));
    listenTo.sin_family = AF_INET;
    listenTo.sin_port = port_;
    listenTo.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socket_, (sockaddr*) &listenTo, sizeof(listenTo)) == -1) {
        QString exception("error binding server socket: ");
        exception.append(strerror(errno));
        throw exception;
    }

    if (listen(socket_, backlog_) == -1) {
        QString exception("error listening on server socket: ");
        exception.append(strerror(errno));
        throw exception;
    }

    QThread::start();
}

Server::~Server() {

    close(socket_);
    running_ = false;

    // Wait for the thread to finish.
    wait();
}

void Server::run() {
    int clientIndex, maxIndex, numReady;
    int newClientSocket, currentClientSocket, maxfd, client[FD_SETSIZE];
    socklen_t client_len;
    sockaddr_in client_addr;
    fd_set rset, allset;
    maxfd	= socket_;	// initialize
    maxIndex	= -1;	// highest index of connections

    for (clientIndex = 0; clientIndex < FD_SETSIZE; clientIndex++) {
        client[clientIndex] = -1;             // -1 indicates available entry
    }

    FD_ZERO(&allset);
    FD_SET(socket_, &allset);

    while (running_) {
        rset = allset;               // structure assignment
        numReady = select(maxfd + 1, &rset, NULL, NULL, NULL);

        if (FD_ISSET(socket_, &rset)) {
            client_len = sizeof(client_addr);

            if ((newClientSocket = accept(socket_, (sockaddr*) &client_addr, &client_len)) == -1) {
                qDebug() << "accept error:" << strerror(errno);
                return; // TODO: inform main window of failure.
            }

            emit signal_clientConnected(QString(inet_ntoa(client_addr.sin_addr)));

            for (clientIndex = 0; clientIndex < FD_SETSIZE; clientIndex++) {
                if (client[clientIndex] < 0) {
                    client[clientIndex] = newClientSocket;	// save descriptor
                    break;
                }
            }

            if (clientIndex == FD_SETSIZE) {
                qDebug() << "Too many clients";
            }

            FD_SET(newClientSocket, &allset);      // add new descriptor to set

            if (newClientSocket > maxfd) {
                maxfd = newClientSocket;	// for select
            }

            if (clientIndex > maxIndex) {
                maxIndex = clientIndex;	 // new max index in client[] array
            }

            if (--numReady <= 0) {
                continue;	// no more readable descriptors
            }
        }

        for (int i = 0; i <= maxIndex; i++)	 {
            if ((currentClientSocket = client[i]) < 0) {
                continue;
            }

            if (FD_ISSET(currentClientSocket, &rset)) {
                ChatMsg chatMsg;

                switch (readMsg(currentClientSocket, chatMsg)) {
                    case kSuccess:
                        switch (chatMsg.type) {
                            case kChat:

                                for (int j = 0; j <= maxIndex; j++) {
                                    if (client[j] == -1) {
                                        continue;
                                    }

                                    if (client[j] == currentClientSocket) {
                                        continue;
                                    }
                                    sendMsg(client[j], chatMsg);
                                }

                                break;
                        }

                        break;
                    case kDisconnect:
                        client_len = sizeof(client_addr);
                        getpeername(client[i], (sockaddr*) &client_addr, &client_len);
                        emit signal_clientDisconnect(QString(inet_ntoa(client_addr.sin_addr)));
                        close(currentClientSocket);
                        FD_CLR(currentClientSocket, &allset);
                        client[i] = -1;
                        break;
                    case kError:
                        break;
                }

                if (--numReady <= 0) {
                    break;        // no more readable descriptors
                }
            }
        }
    }
}
