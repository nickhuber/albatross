#ifndef SERVER_H
#define SERVER_H

class Server {

private:
    int port_;

public:
    Server(int port);

private:
    void startServer();
};

#endif // SERVER_H
