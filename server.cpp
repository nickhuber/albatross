#include <sys/socket.h>
#include <arpa/inet.h>
#include <QDebug>

#include "server.h"


Server::Server(uint16_t port) : port_(htons(port)) {
    backlog_ = 5;
    socket_ = socket(PF_INET, SOCK_STREAM, 0);
    startServer();
}

Server::~Server() {
    close(socket_);
}

void Server::startServer() {

	sockaddr_in listenTo;
	listenTo.sin_family = AF_INET;
	listenTo.sin_port = port_;
	listenTo.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(socket_, (sockaddr*) &listenTo, sizeof(listenTo)) == -1) {
		qDebug() << "error binding server socket";
	}

	if (listen(socket_, backlog_) == -1) {
		qDebug() << "error listening on server socket";
	}
}

void Server::serving() {

}
