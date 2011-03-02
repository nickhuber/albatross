#include <arpa/inet.h>

#include "client.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->startPushButton, SIGNAL(clicked()), SLOT(slot_start()));
    connect(ui->connectPushButton, SIGNAL(clicked()), SLOT(slot_connect()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slot_start() {
    ui->tabWidget->setTabEnabled(0, false);
}

void MainWindow::slot_connect() {
    ui->tabWidget->setTabEnabled(1, false);
    uint32_t ip = htonl(ui->serverIPLineEdit->text().toUInt());
    uint16_t port = htons(ui->portLineEdit->text().toUInt());
    Client client = Client(ip, port, ui->usernameLineEdit->text());
}
