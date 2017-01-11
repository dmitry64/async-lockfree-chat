#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <iostream>
#include <iomanip>
#include <deque>
#include <boost/asio.hpp>
#include "ChatMessage.pb.h"
#include "google/protobuf/io/zero_copy_stream.h"
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"
#include <boost/lexical_cast.hpp>

class ChatClient
{
    boost::asio::io_service& _io_service;
    boost::asio::ip::tcp::socket _socket;
    char _temp;
    std::vector<google::protobuf::uint8> _headerBuffer;
    std::vector<google::protobuf::uint8> _bodyBuffer;

public:
    ChatClient(boost::asio::io_service& io_service,
               boost::asio::ip::tcp::resolver::iterator end_iterator)
        : _io_service(io_service),
          _socket(io_service)
    {
        do_connect(end_iterator);
    }

    std::vector<google::protobuf::uint8> createBufferFromMessage(const ChatMessage::ChatMessage & message);
    void write(const ChatMessage::ChatMessage& msg);
    void close();
    void tryReadHeader();
    void tryReadBody();

private:
    void do_connect(boost::asio::ip::tcp::resolver::iterator end_iterator)
    {
        boost::asio::async_connect(_socket, end_iterator,
        [this](boost::system::error_code ec, boost::asio::ip::tcp::resolver::iterator) {
            if (!ec) {
                tryReadHeader();
            }
        });
    }
};

#endif // CHATCLIENT_H
