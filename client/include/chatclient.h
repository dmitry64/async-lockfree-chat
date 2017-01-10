#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <iostream>
#include <deque>
#include <boost/asio.hpp>
#include "ChatMessage.pb.h"
#include "google/protobuf/io/zero_copy_stream.h"
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"
#include <boost/lexical_cast.hpp>

class ChatClient
{
    std::deque< std::pair<char *, unsigned int> > _encodedMessages;
    boost::asio::io_service& io_service_;
    boost::asio::ip::tcp::socket _socket;
    unsigned int _currentMessageSize;
    unsigned char * _currentMessageBuffer;
    char _temp;
    std::vector<google::protobuf::uint8> _bytesArray;
    std::pair<char *, unsigned int> test;
public:
    ChatClient(boost::asio::io_service& io_service,
               boost::asio::ip::tcp::resolver::iterator end_iterator)
        : io_service_(io_service),
          _socket(io_service)
    {
        ChatMessage::ChatMessage message;
        message.set_sender("Sender");
        message.set_text("Text here" + boost::lexical_cast<std::string>(2017));
        auto pair = createBufferFromMessage(message);
        //_encodedMessages.push_back(pair);
        test = pair;

        do_connect(end_iterator);
    }

    std::pair<char *, unsigned int> createBufferFromMessage(const ChatMessage::ChatMessage & message);

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
                do_write();
                tryReadHeader();
            }
        });
    }
    void do_write();


};

#endif // CHATCLIENT_H
