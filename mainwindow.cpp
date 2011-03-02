#include <arpa/inet.h>
#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "server.h"
#include "client.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/albatross.png"));
    connect(ui->startPushButton, SIGNAL(clicked()), SLOT(slot_start()));
    connect(ui->connectPushButton, SIGNAL(clicked()), SLOT(slot_connect()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slot_start() {
    bool validNum;
    uint port;
    Server* server;

    port = ui->portServerLineEdit->text().toUInt(&validNum);    // TODO: check to see if it fails
    server = new Server(htons(port));
    ui->startPushButton->setText("Stop");
    ui->tabWidget->setTabEnabled(0, false);
}

void MainWindow::slot_connect() {
    bool validNum;
    in_addr_t ip;
    uint16_t port;
    Client* client;

    // disable the server tab item
    ui->tabWidget->setTabEnabled(1, false);

    if ((ip = inet_addr(ui->serverIPLineEdit->text().toAscii())) == INADDR_NONE) {
        qDebug() << "Bad IP address";
        return;
    }

    port = ui->portLineEdit->text().toUInt(&validNum);
    if (!validNum) {
        qDebug() << "Bad port number";
        return;
    }

    client = new Client(ip, htons(port), ui->usernameLineEdit->text());

    ui->connectPushButton->setText(tr("Disconnect"));
}
