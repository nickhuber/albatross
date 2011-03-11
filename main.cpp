#include <QtGui/QApplication>

#ifdef _WIN32
#include <winsock2.h>
#endif

#include "mainwindow.h"

int main(int argc, char *argv[]) {
#ifdef _WIN32
    WSADATA wsaData;

    if (WSAStartup(0x0202, &wsaData) != 0) {
        WSACleanup();
        return 1;
    }
#endif

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    int retCode = a.exec();

#ifdef _WIN32
    WSACleanup();
#endif
    return retCode;

}
