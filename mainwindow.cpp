
#ifndef _WIN32
#include <arpa/inet.h>
#endif

#ifdef _WIN32
#include <winsock2.h>
#endif

#include <QDebug>
#include <QColor>
#include <QFile>
#include <QTextStream>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "server.h"
#include "client.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setClientGuiVisible(true);
    setWindowIcon(QIcon(":/albatross.png"));
    setWindowTitle(tr("Albatross"));
    connect(ui->startPushButton, SIGNAL(clicked()), SLOT(slot_start()));
    connect(ui->connectPushButton, SIGNAL(clicked()), SLOT(slot_connect()));
    connect(ui->sendPushButton, SIGNAL(clicked()), SLOT(slot_send()));
    connect(ui->sendLineEdit, SIGNAL(returnPressed()), SLOT(slot_send()));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::slot_start() {
    bool validNum;
    uint port;
    port = ui->portServerLineEdit->text().toUInt(&validNum);

    if (!validNum) {
        qDebug() << "Bad port number";
        return;
    }

    try {
        server_ = new Server(htons(port));
    } catch (const QString& e) {
        qDebug() << e;
        return;
    }

    connect(server_, SIGNAL(signal_clientConnected(QString)), SLOT(slot_connectedClient(QString)));
    connect(server_, SIGNAL(signal_clientDisconnect(int)), SLOT(slot_disconnectedClient(int)));
    ui->startPushButton->setText("Stop");
    ui->tabWidget->setTabEnabled(0, false);
    ui->startPushButton->disconnect();
    connect(ui->startPushButton, SIGNAL(clicked()), SLOT(slot_stop()));
}

void MainWindow::slot_stop() {
    disconnect(server_, SIGNAL(signal_clientConnected(QString)));
    disconnect(server_, SIGNAL(signal_clientDisconnect(int)));
    delete server_;
    ui->startPushButton->setText("Start");
    ui->tabWidget->setTabEnabled(0, true);
    ui->startPushButton->disconnect();
    connect(ui->startPushButton, SIGNAL(clicked()), SLOT(slot_start()));
}

void MainWindow::slot_connect() {
    bool validNum;
    in_addr_t ip;
    uint16_t port;

    if (ui->usernameLineEdit->text().size() == 0) {
        qDebug() << "No username entered";
        return;
    }

    if ((ip = inet_addr(ui->serverIPLineEdit->text().toAscii())) == INADDR_NONE) {
        qDebug() << "Bad IP address";
        return;
    }

    port = ui->portLineEdit->text().toUInt(&validNum);

    if (!validNum) {
        qDebug() << "Bad port number";
        return;
    }

    try {
        client_ = new Client(ip, htons(port), ui->usernameLineEdit->text());
    } catch (const QString& e) {
        qDebug() << e;
        return;
    }

    // if the checkbox is set, save to a file.
    if (ui->saveSessionCheckBox->checkState() == Qt::Checked) {
        connect(client_, SIGNAL(signal_messageReceived(QString, QString)), SLOT(slot_saveToFile(QString, QString)));
    }
    // when a message is received, print to the output
    connect(client_, SIGNAL(signal_messageReceived(QString, QString)), SLOT(slot_displayMessage(QString, QString)));

    // if the server shuts down, disconnect the client
    connect(client_, SIGNAL(signal_disconnected()), SLOT(slot_disconnect()));

    setClientGuiVisible(false);
    ui->connectPushButton->setText(tr("Disconnect"));
    ui->tabWidget->setTabEnabled(1, false);
    ui->connectPushButton->disconnect();
    connect(ui->connectPushButton, SIGNAL(clicked()), SLOT(slot_disconnect()));
}

void MainWindow::slot_disconnect() {

    disconnect(client_, SIGNAL(signal_messageReceived(QString, QString)));
    disconnect(client_, SIGNAL(signal_disconnected()));
    delete client_;
    setClientGuiVisible(true);
    ui->connectPushButton->setText(tr("Connect"));
    ui->tabWidget->setTabEnabled(1, true);
    ui->connectPushButton->disconnect();
    connect(ui->connectPushButton, SIGNAL(clicked()), SLOT(slot_connect()));
}

void MainWindow::slot_send() {
    QString message = ui->sendLineEdit->text();
    ui->sendLineEdit->clear();
    client_->sendMsg(kChat, message.size() + 1, message.toStdString().c_str());
}

void MainWindow::slot_displayMessage(const QString& username, const QString& message) {
    int sum = 0;

    // generate a number from the username
    for (int i = 0; i < username.size(); i++) {
        sum += username[i].toAscii();
    }

    // Construct a colour from that number.
    // 3 and 7 are numbers just to make sure the colour isnt always gray
    QColor colour(sum % 255, sum * 3 % 255, sum * 7 % 255);

    ui->chatMessagesText->append("<span style='color:" + colour.name() + "'>" + username + "</span>" + ": " + message);
}

void MainWindow::slot_saveToFile(const QString& username, const QString& message) {
    QFile file("log.txt");
    file.open(QFile::WriteOnly | QFile::Append);
    QTextStream stream(&file);
    stream << username << ": " << message << "\r\n";
}

void MainWindow::slot_connectedClient(const QString &address) {
    ui->clientList->addItem(address);
}

void MainWindow::slot_disconnectedClient(const int index) {
    ui->clientList->takeItem(index);
}

void MainWindow::setClientGuiVisible(bool visible) {

    ui->saveSessionLabel->setVisible(visible);
    ui->saveSessionCheckBox->setVisible(visible);
    ui->serverIPLabel->setVisible(visible);
    ui->serverIPLineEdit->setVisible(visible);
    ui->portLabel->setVisible(visible);
    ui->portLineEdit->setVisible(visible);
    ui->usernameLineEdit->setEnabled(visible);
    ui->chatMessagesText->setVisible(!visible);
    ui->sendLineEdit->setVisible(!visible);
    ui->sendPushButton->setVisible(!visible);
}
