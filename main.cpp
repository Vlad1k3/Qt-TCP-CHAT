#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include <boost/asio.hpp>

boost::asio::io_service service;
boost::asio::streambuf buf;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
