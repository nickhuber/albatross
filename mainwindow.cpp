#include <arpa/inet.h>
#include <QDebug>
#include "client.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "server.h"

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
    uint32_t ip;
    uint16_t port;

    // disable the server tab item
    ui->tabWidget->setTabEnabled(1, false);

    ip = htonl(ui->serverIPLineEdit->text().toUInt(&validNum));
    port = htons(ui->portLineEdit->text().toUInt(&validNum));

    if (!validNum) {
        qDebug() << "Bad number";
        return;
    }

    Client client = Client(ip, port, ui->usernameLineEdit->text());

    ui->connectPushButton->setText(tr("Disconnect"));
}
