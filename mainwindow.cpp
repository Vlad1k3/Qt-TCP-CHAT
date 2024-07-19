#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "messagedelegate.h"
#include <QMessageBox>
#include <QKeyEvent>
#include <boost/asio.hpp>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , socket(service)
    , readBuffer(std::make_unique<boost::asio::streambuf>())
    , asioTimer(new QTimer(this))
{
    ui->setupUi(this);

    // Создание нового CustomTextEdit и замена textEdit для реагирование на нажатие Enter
    CustomTextEdit *customTextEdit = new CustomTextEdit(this);
    ui->verticalLayout->replaceWidget(ui->textEdit, customTextEdit);
    delete ui->textEdit;
    ui->textEdit = customTextEdit;

    // Установка кастомного делегата
    ui->listWidget->setItemDelegate(new MessageDelegate(this));

    sWindow = new Registration();
    connect(sWindow, &Registration::firstWindow, this, &MainWindow::show);

    // Подключение к серверу
    boost::asio::ip::tcp::resolver resolver(service);
    boost::asio::ip::tcp::resolver::query query("localhost", "8080");
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    boost::asio::connect(socket, endpoint_iterator);

    // Запуск асинхронного чтения сообщений от сервера
    startReading();

    // Подключение сигнала к слоту
    connect(this, &MainWindow::newMessageReceived, this, &MainWindow::onNewMessageReceived);

    // Подключение таймера к слоту для обработки событий Asio
    connect(asioTimer, &QTimer::timeout, this, &MainWindow::processAsioEvents);
    asioTimer->start(10);

    // Подключение сигнала от кастомного текстового поля
    connect(customTextEdit, &CustomTextEdit::enterPressed, this, &MainWindow::on_pushButton_clicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//кнопка отправка сообщения на сервер
void MainWindow::on_pushButton_clicked()
{
    if (username != ""){
        QString message = ui->textEdit->toPlainText();
        if (message.isEmpty()){
            QMessageBox::information(this, "Alert", "Enter line is empty");
        } else {
            message = username + " : " + message + "\n";
            boost::asio::write(socket, boost::asio::buffer(message.toStdString()));
            addMessageToChat(message);
        }
        ui->textEdit->clear();
    }
    else{
        QMessageBox::information(this, "Alert", "Please, login");
    }
}

//чтение данных с сервера.
void MainWindow::startReading()
{
    boost::asio::async_read_until(socket, *readBuffer, '\n',
                                  [this](const boost::system::error_code& error, std::size_t bytes_transferred)
                                  {
                                      if (!error) {
                                          std::istream is(readBuffer.get());
                                          std::string line;
                                          std::getline(is, line);
                                          QString qmsg = QString::fromStdString(line);

                                          // Передача сообщения в главный поток для обновления UI
                                          emit newMessageReceived(qmsg);
                                          startReading();
                                      } else {
                                          std::cerr << "Ошибка чтения: " << error.message() << std::endl;
                                      }
                                  });
}

//кнопка логирования
void MainWindow::on_pushButton_2_clicked()
{
    sWindow->show();
}

void MainWindow::addMessageToChat(QString message)
{
    QListWidgetItem* item = new QListWidgetItem(message);
    item->setSizeHint(QSize(ui->listWidget->width(), 20)); // Задайте желаемую высоту
    ui->listWidget->addItem(item);
    ui->listWidget->scrollToBottom();
}

void MainWindow::onNewMessageReceived(QString message)
{
    addMessageToChat(message);
}

void MainWindow::processAsioEvents()
{
    service.poll();
}

void MainWindow::keyPressEvent(QKeyEvent *event){
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter){
        on_pushButton_clicked();
    }
}
