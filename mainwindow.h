#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QTextEdit>
#include <QKeyEvent>
#include <QListWidget>
#include <boost/asio.hpp>
#include "registration.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class CustomTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    explicit CustomTextEdit(QWidget *parent = nullptr) : QTextEdit(parent) {}

signals:
    void enterPressed();

protected:
    void keyPressEvent(QKeyEvent *event) override {
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            emit enterPressed();
        } else {
            QTextEdit::keyPressEvent(event);
        }
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void onNewMessageReceived(QString message);
    void processAsioEvents(); // Новый слот для обработки событий Asio

signals:
    void newMessageReceived(QString message);

private:
    Ui::MainWindow *ui;
    boost::asio::io_service service;
    boost::asio::ip::tcp::socket socket;
    Registration *sWindow;
    std::unique_ptr<boost::asio::streambuf> readBuffer;
    QTimer *asioTimer; // Новый QTimer

    void startReading();
    void addMessageToChat(QString message);
};

#endif // MAINWINDOW_H
