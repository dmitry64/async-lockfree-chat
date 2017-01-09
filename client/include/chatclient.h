#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <iostream>
#include <deque>
#include <boost/asio.hpp>
#include "ChatMessage.pb.h"
#include "google/protobuf/io/zero_copy_stream.h"
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"


class ChatClient
{
    std::deque< std::pair<char *, unsigned int> > _encodedMessages;
    boost::asio::io_service& io_service_;
    boost::asio::ip::tcp::socket socket_;
public:
    ChatClient(boost::asio::io_service& io_service,
               boost::asio::ip::tcp::resolver::iterator end_iterator)
        : io_service_(io_service),
          socket_(io_service)
    {
        ChatMessage::ChatMessage message;
        message.set_sender("Sender");
        message.set_text("Text here");
        auto pair = createBufferFromMessage(message);
        _encodedMessages.push_back(pair);

        do_connect(end_iterator);
    }

    std::pair<char *, unsigned int> createBufferFromMessage(const ChatMessage::ChatMessage & message);

    void write(const ChatMessage::ChatMessage& msg);
    void close();

private:
    void do_connect(boost::asio::ip::tcp::resolver::iterator end_iterator)
    {
        boost::asio::async_connect(socket_, end_iterator,
        [this](boost::system::error_code ec, boost::asio::ip::tcp::resolver::iterator) {
            if (!ec) {
                do_write();
            }
        });
    }
    void do_write();


};

#endif // CHATCLIENT_H
