#include <arpa/inet.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "server.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->startPushButton, SIGNAL(clicked()), SLOT(slot_start()));
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
    ui->portServerLineEdit->setText("Stop");
    ui->tabWidget->setTabEnabled(0, false);
}
