#include <sys/socket.h>
#include <arpa/inet.h>

// debugging
#include <QDebug>
#include <errno.h>
#include <string.h>

#include "server.h"
#include "chatmsg.h"

Server::Server(uint16_t port) : socket_(0), port_(port), backlog_(5), running_(true) {

    if ((socket_ = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        qDebug() << "error creating server socket:" << strerror(errno);
        throw;
    }

    int arg = 1;
    if (setsockopt (socket_, SOL_SOCKET, SO_REUSEADDR, &arg, sizeof(arg)) == -1) {
        qDebug() << "error setting up socket:" << strerror(errno);
        throw;
    }

    sockaddr_in listenTo;
    bzero((char *) &listenTo, sizeof(listenTo));
    listenTo.sin_family = AF_INET;
    listenTo.sin_port = port_;
    listenTo.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socket_, (sockaddr*) &listenTo, sizeof(listenTo)) == -1) {
        qDebug() << listenTo.sin_addr.s_addr << listenTo.sin_family << listenTo.sin_port << socket_;
        qDebug() << "error binding server socket:" << strerror(errno);
        throw;
    }

    if (listen(socket_, backlog_) == -1) {
        qDebug() << "error listening on server socket:" << strerror(errno);
        throw;
    }


    QThread::start();
}

Server::~Server() {
   shutdown(socket_, SHUT_RDWR);
   running_ = false;
   // TODO: convert this to a mutex
   while(isRunning()) { // wait for thread to exit
       ;
   }
}

void Server::run() {
    int clientIndex, maxIndex, numReady;
    int newClientSocket, currentClientSocket, maxfd, client[FD_SETSIZE];
    socklen_t client_len;
    sockaddr_in client_addr;
    fd_set rset, allset;

    maxfd	= socket_;	// initialize
    maxIndex	= -1;		// index into client[] array

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
            if ((newClientSocket = accept(socket_, (sockaddr *) &client_addr, &client_len)) == -1) {
                qDebug() << "accept error:" << strerror(errno);
                return; // TODO: inform main window of failure.
            }

            printf(" Remote Address:  %s\n", inet_ntoa(client_addr.sin_addr));

            for (clientIndex = 0; clientIndex < FD_SETSIZE; clientIndex++) {
                if (client[clientIndex] < 0) {
                    client[clientIndex] = newClientSocket;	// save descriptor
                    break;
                }
            }
            if (clientIndex == FD_SETSIZE) {
                qDebug() << "Too many clients";
            }

            FD_SET (newClientSocket, &allset);     // add new descriptor to set
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
                    qDebug() << chatMsg.data;

                    switch (chatMsg.type) {
                    case kChat:
                        for (int j = 0; j <= maxIndex; j++) {
                            if (client[j] == -1) {
                                continue;
                            }
                            if (client[j] == currentClientSocket) {
                                continue;
                            }
                            qDebug() << "sending";
                            sendMsg(client[j], chatMsg);

                        }
                        break;
                    }

                    break;
                case kDisconnect:
                    qDebug() << "connection closed";
                    shutdown(currentClientSocket, SHUT_RDWR);
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
