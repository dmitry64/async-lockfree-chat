#ifndef CHATSERVER_H
#define CHATSERVER_H
#include "chatsession.h"
#include <boost/asio.hpp>

class ChatServer
{
    boost::asio::ip::tcp::acceptor _acceptor;
    boost::asio::ip::tcp::socket _socket;
public:
    ChatServer(boost::asio::io_service& io_service, const boost::asio::ip::tcp::endpoint& endpoint);
private:
    void doAccept();
};

#endif // CHATSERVER_H
