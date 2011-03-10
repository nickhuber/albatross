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
        throw "error";
    }

    int arg = 1;
    if (setsockopt (socket_, SOL_SOCKET, SO_REUSEADDR, &arg, sizeof(arg)) == -1) {
        qDebug() << "error setting up socket:" << strerror(errno);
        throw "error";
    }

    sockaddr_in listenTo;
    bzero((char *) &listenTo, sizeof(listenTo));
    listenTo.sin_family = AF_INET;
    listenTo.sin_port = port_;
    listenTo.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socket_, (sockaddr*) &listenTo, sizeof(listenTo)) == -1) {
        qDebug() << listenTo.sin_addr.s_addr << listenTo.sin_family << listenTo.sin_port << socket_;
        qDebug() << "error binding server socket:" << strerror(errno);
        throw "error";
    }

    if (listen(socket_, backlog_) == -1) {
        qDebug() << "error listening on server socket:" << strerror(errno);
        throw "error";
    }


    QThread::start();
}

Server::~Server() {
   close(socket_);
   running_ = false;
   // TODO: convert this to a mutex
   while(isRunning()) { // wait for thread to exit
       ;
   }
}

void Server::run() {
    int i, maxi, nready, bytes_to_read;
    int new_sd, sockfd, maxfd, client[FD_SETSIZE];
    socklen_t client_len;
    struct sockaddr_in client_addr;
    ssize_t n;
    fd_set rset, allset;

    maxfd	= socket_;	// initialize
    maxi	= -1;		// index into client[] array

    for (i = 0; i < FD_SETSIZE; i++) {
        client[i] = -1;             // -1 indicates available entry
    }

    FD_ZERO(&allset);
    FD_SET(socket_, &allset);

    while (true) {
        rset = allset;               // structure assignment
        nready = select(maxfd + 1, &rset, NULL, NULL, NULL);

        if (FD_ISSET(socket_, &rset)) {
            client_len = sizeof(client_addr);
            if ((new_sd = accept(socket_, (struct sockaddr *) &client_addr, &client_len)) == -1) {
                qDebug() << "accept error:" << strerror(errno);
                return; // TODO: inform main window of failure.
            }

            printf(" Remote Address:  %s\n", inet_ntoa(client_addr.sin_addr));

            for (i = 0; i < FD_SETSIZE; i++) {
                if (client[i] < 0) {
                    client[i] = new_sd;	// save descriptor
                    break;
                }
            }
            if (i == FD_SETSIZE) {
                printf ("Too many clients\n");
                exit(1);
            }

            FD_SET (new_sd, &allset);     // add new descriptor to set
            if (new_sd > maxfd) {
                maxfd = new_sd;	// for select
            }

            if (i > maxi) {
                maxi = i;	// new max index in client[] array
            }

            if (--nready <= 0) {
                continue;	// no more readable descriptors
            }
        }

        for (int i = 0; i <= maxi; i++)	 {
            if ((sockfd = client[i]) < 0) {
                continue;
            }

            if (FD_ISSET(sockfd, &rset)) {
                ChatMsg chatMsg;
                bytes_to_read = sizeof(chatMsg.size);
                char* buffer = new char[bytes_to_read];
                char* buffer_head = buffer;

                // read the message size

                while ((n = read(sockfd, buffer, bytes_to_read)) > 0) {
                    buffer += n;
                    bytes_to_read -= n;
                }

                if (n == -1) {
                    qDebug() << "error reading:" << strerror(errno);
                    break;
                }

                // extra check on first read, if we read nothing then it was a disconnect
                if (n == 0 && bytes_to_read != 0) {
                    printf(" Remote Address:  %s closed connection\n", inet_ntoa(client_addr.sin_addr));
                    close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
                    break;
                }

                chatMsg.size = (size_t) *buffer_head;

                // read the message type

                delete[] buffer_head;

                bytes_to_read = sizeof(chatMsg.type);
                buffer = new char[bytes_to_read];
                buffer_head = buffer;

                while ((n = read(sockfd, buffer, bytes_to_read)) > 0) {
                    buffer += n;
                    bytes_to_read -= n;
                }

                if (n == -1) {
                    qDebug() << "error reading:" << strerror(errno);
                    break;
                }

                chatMsg.type = (MsgType) *buffer_head;

                // read the message data

                delete[] buffer_head;

                bytes_to_read = chatMsg.size;
                buffer = new char[bytes_to_read];
                buffer_head = buffer;

                while ((n = read(sockfd, buffer, bytes_to_read)) > 0) {
                    buffer += n;
                    bytes_to_read -= n;
                }

                if (n == -1) {
                    qDebug() << "error reading:" << strerror(errno);
                    break;
                }

                chatMsg.data = buffer_head;

                qDebug() << chatMsg.data;

                if (--nready <= 0) {
                    break;        // no more readable descriptors
                }
            }
        }
    }

}
