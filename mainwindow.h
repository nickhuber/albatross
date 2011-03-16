#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "client.h"
#include "server.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow* ui;
    Client* client_;
    Server* server_;

    /**
      Hides or shows the client GUI components.

      @param visible whether or not to show the client GUI stuff.
      @author Nick Huber
      */
    void setClientGuiVisible(bool visible);

public slots:
    /**
      Slot for when the start button is clicked on the server tab.

      @author Dean Morin
      */
    void slot_start();

    /**
      Slot for when the stop button is clicked on the server tab.

      @author Dean Morin
      */
    void slot_stop();

    /**
      Slot for when the connect button is clicked on the client tab.

      @author Nick Huber
      */
    void slot_connect();

    /**
      Slot for when the disconnect button is clicked on the client tab.

      @author Nick Huber
      */
    void slot_disconnect();

    /**
      Slot for when the send button is clicked when a client is connected.

      @author Nick Huber
      */
    void slot_send();

    /**
      Slot for displaying a message. Updates the text window with the message.

      @param username The sender's username.
      @param message The sender's message.
      @author Nick Huber
      */
    void slot_displayMessage(const QString& username, const QString& message);

    /**
      Slot for writing a message to a file. appends into log.txt in the current directory.

      @param username The sender's username.
      @param message The sender's message.
      @author Nick Huber
      */
    void slot_saveToFile(const QString& username, const QString& message);

    /**
      Slot for when a client is connected to update the server list.

      @param address The client's IP address.
      @author Nick Huber
      */
    void slot_connectedClient(const QString& address);

    /**
      Slot for when a client disconnects from the server, to remove from the list.

      @param address The IP of the disconnected client
      @author Nick Huber
      */
    void slot_disconnectedClient(const QString& address);
};

#endif // MAINWINDOW_H
