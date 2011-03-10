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
    setClientGuiVisible(true);

    setWindowIcon(QIcon(":/albatross.png"));
    setWindowTitle(tr("Albatross"));
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

    port = ui->portServerLineEdit->text().toUInt(&validNum);    // TODO: check to see if it fails
    try {
        server = new Server(htons(port));
    } catch(const char*) {
        return;
    }

    ui->startPushButton->setText("Stop");
    ui->tabWidget->setTabEnabled(0, false);
    ui->startPushButton->disconnect();
    connect(ui->startPushButton, SIGNAL(clicked()), SLOT(slot_stop()));
}

void MainWindow::slot_stop() {
    delete server;
    ui->startPushButton->setText("Start");
    ui->tabWidget->setTabEnabled(0, true);
    ui->startPushButton->disconnect();
    connect(ui->startPushButton, SIGNAL(clicked()), SLOT(slot_start()));
}

void MainWindow::slot_connect() {
    bool validNum;
    in_addr_t ip;
    uint16_t port;

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

    setClientGuiVisible(false);

    ui->connectPushButton->setText(tr("Disconnect"));
    ui->tabWidget->setTabEnabled(1, false);
    ui->connectPushButton->disconnect();
    connect(ui->connectPushButton, SIGNAL(clicked()), SLOT(slot_disconnect()));
}

void MainWindow::slot_disconnect() {
    delete client;

    setClientGuiVisible(true);

    ui->connectPushButton->setText(tr("Connect"));
    ui->tabWidget->setTabEnabled(1, true);
    ui->connectPushButton->disconnect();
    connect(ui->connectPushButton, SIGNAL(clicked()), SLOT(slot_connect()));
}

void MainWindow::setClientGuiVisible(bool visible) {
    ui->saveSessionLabel->setVisible(visible);
    ui->saveSessionCheckBox->setVisible(visible);
    ui->serverIPLabel->setVisible(visible);
    ui->serverIPLineEdit->setVisible(visible);
    ui->portLabel->setVisible(visible);
    ui->portLineEdit->setVisible(visible);
    ui->usernameLineEdit->setEnabled(visible);

    ui->receivedMsgScrollArea->setVisible(!visible);
    ui->sendLineEdit->setVisible(!visible);
    ui->sendPushButton->setVisible(!visible);
}
