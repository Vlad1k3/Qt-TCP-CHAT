#ifndef CLIENT_H
#define CLIENT_H

#include <boost/asio.hpp>

extern boost::asio::io_service service;

extern boost::asio::streambuf buf;

void read(boost::asio::ip::tcp::socket& socket);

#endif // CLIENT_H
