#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    /**
      Hides or shows the client GUI components.

      @param visible whether or not to show the client GUI stuff.
      @author Nick Huber
      */
    void setClientGuiVisible(bool visible);

public slots:
    void slot_start();
    void slot_stop();
    void slot_connect();
    void slot_disconnect();
};

#endif // MAINWINDOW_H
