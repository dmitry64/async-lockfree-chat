#include "chatserver.h"


ChatServer::ChatServer(boost::asio::io_service &io_service, const boost::asio::ip::tcp::endpoint &endpoint)
    : _acceptor(io_service, endpoint),
      _socket(io_service)
{
    doAccept();
}

void ChatServer::doAccept()
{
    _acceptor.async_accept(_socket,
    [this](boost::system::error_code ec) {
        if (!ec) {
            std::make_shared<ChatSession>(std::move(_socket))->start();
        }
        doAccept();
    });
}
