
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
    connect(server_, SIGNAL(signal_clientDisconnect(QString)), SLOT(slot_disconnectedClient(QString)));
    ui->startPushButton->setText("Stop");
    ui->tabWidget->setTabEnabled(0, false);
    ui->startPushButton->disconnect();
    connect(ui->startPushButton, SIGNAL(clicked()), SLOT(slot_stop()));
}

void MainWindow::slot_stop() {
    disconnect(server_, SIGNAL(signal_clientConnected(QString)));
    disconnect(server_, SIGNAL(signal_clientDisconnect(QString)));
    delete server_;
    ui->clientList->clear();
    ui->startPushButton->setText("Start");
    ui->tabWidget->setTabEnabled(0, true);
    ui->startPushButton->disconnect();
    connect(ui->startPushButton, SIGNAL(clicked()), SLOT(slot_start()));
}

void MainWindow::slot_connect() {
    bool validNum;
    in_addr_t ip;
    uint16_t port;
    bool success = true;

    ui->ip_error->setText("");
    ui->username_error->setText("");
    ui->port_error->setText("");
    ui->connect_error->setText("");

    if (ui->usernameLineEdit->text().size() == 0) {
        ui->username_error->setText("Required");
        success = false;
    }

    if ((ui->serverIPLineEdit->text().size() == 0)) {
        ui->ip_error->setText("Required");
        success = false;
    } else if ((ip = inet_addr(ui->serverIPLineEdit->text().toAscii())) == INADDR_NONE) {
        ui->ip_error->setText("Invalid");
        success = false;
    }

    port = ui->portLineEdit->text().toUInt(&validNum);
    if (ui->portLineEdit->text().size() == 0) {
        ui->port_error->setText("Required");
        success = false;
    } else if (!validNum) {
        ui->port_error->setText("Invalid");
        success = false;
    }

    if (!success) {
        return;
    }

    try {
        client_ = new Client(ip, htons(port), ui->usernameLineEdit->text());
    } catch (const QString& e) {
        ui->connect_error->setText(e);
        return;
    }

    // if the checkbox is set, save to a file.
    if (ui->saveSessionCheckBox->checkState() == Qt::Checked) {
        connect(client_, SIGNAL(signal_messageReceived(QString, QString, QString)), SLOT(slot_saveToFile(QString, QString, QString)));
    }
    // when a message is received, print to the output
    connect(client_, SIGNAL(signal_messageReceived(QString, QString, QString)), SLOT(slot_displayMessage(QString, QString, QString)));

    // if the server shuts down, disconnect the client
    connect(client_, SIGNAL(signal_disconnected()), SLOT(slot_disconnect()));

    setClientGuiVisible(false);
    ui->connectPushButton->setText(tr("Disconnect"));
    ui->tabWidget->setTabEnabled(1, false);
    ui->connectPushButton->disconnect();
    connect(ui->connectPushButton, SIGNAL(clicked()), SLOT(slot_disconnect()));
}

void MainWindow::slot_disconnect() {

    disconnect(client_, SIGNAL(signal_messageReceived(QString, QString, QString)));
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

void MainWindow::slot_displayMessage(const QString& username, const QString& address, const QString& message) {
    int sum = 0;

    // generate a number from the username
    for (int i = 0; i < username.size(); i++) {
        sum += username[i].toAscii();
    }

    // Construct a colour from that number.
    // 3 and 7 are numbers just to make sure the colour isnt always gray
    QColor colour(sum % 255, sum * 3 % 255, sum * 7 % 255);

    ui->chatMessagesText->append("<span style='color:" + colour.name() + "'>" + username + "</span>" + "(" + address + ")" + ": " + message);
}

void MainWindow::slot_saveToFile(const QString& username, const QString& address, const QString& message) {
    QFile file("log.txt");
    file.open(QFile::WriteOnly | QFile::Append);
    QTextStream stream(&file);
    stream << username << "(" << address << ")" << ": " << message << "\r\n";
}

void MainWindow::slot_connectedClient(const QString &address) {
    ui->clientList->addItem(address);
}

void MainWindow::slot_disconnectedClient(const QString& address) {
    qDeleteAll(ui->clientList->findItems(address, Qt::MatchExactly));
}

void MainWindow::setClientGuiVisible(bool visible) {

    ui->saveSessionLabel->setEnabled(visible);
    ui->saveSessionCheckBox->setEnabled(visible);
    ui->serverIPLabel->setEnabled(visible);
    ui->serverIPLineEdit->setEnabled(visible);
    ui->portLabel->setEnabled(visible);
    ui->portLineEdit->setEnabled(visible);
    ui->usernameLineEdit->setEnabled(visible);
    ui->chatMessagesText->setVisible(!visible);
    ui->sendLineEdit->setVisible(!visible);
    ui->sendPushButton->setVisible(!visible);
}
