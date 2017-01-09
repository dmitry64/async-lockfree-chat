#ifndef CHATSERVER_H
#define CHATSERVER_H
#include "chatsession.h"
#include <boost/asio.hpp>

class ChatServer
{
public:
    ChatServer(boost::asio::io_service& io_service,
                const boost::asio::ip::tcp::endpoint& endpoint)
        : acceptor_(io_service, endpoint),
          socket_(io_service)
    {
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept(socket_,
        [this](boost::system::error_code ec) {
            if (!ec) {
                std::make_shared<ChatSession>(std::move(socket_))->start();
            }
            do_accept();
        });
    }

    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::socket socket_;
};

#endif // CHATSERVER_H
