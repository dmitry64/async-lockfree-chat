#include "chatserver.h"


ChatServer::ChatServer(boost::asio::io_service &io_service, const boost::asio::ip::tcp::endpoint &endpoint)
    : _acceptor(io_service, endpoint),
      _socket(io_service),
      _io_service(io_service),
      _room(new ChatRoom())
{
    doAccept();
}

ChatServer::~ChatServer()
{
    _acceptor.cancel();
    delete _room;
}

void ChatServer::doAccept()
{
    _acceptor.async_accept(_socket,
    [this](boost::system::error_code ec) {
        if (!ec) {
            std::make_shared<ChatSession>(_io_service,std::move(_socket),_room)->start();
        }
        doAccept();
    });
}
