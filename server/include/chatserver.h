#ifndef CHATSERVER_H
#define CHATSERVER_H
#include "chatsession.h"
#include "chatroom.h"
#include <boost/asio.hpp>

class ChatServer
{
    boost::asio::ip::tcp::acceptor _acceptor;
    boost::asio::ip::tcp::socket _socket;
    boost::asio::io_service& _io_service;
public:
    ChatServer(boost::asio::io_service& io_service, const boost::asio::ip::tcp::endpoint& endpoint);
    ~ChatServer();
private:
    void doAccept();
    ChatRoom * _room;
};

#endif // CHATSERVER_H
