#ifndef CHATSESSION_H
#define CHATSESSION_H

#include <iostream>
#include <boost/asio.hpp>
#include "ChatMessage.pb.h"
#include "google/protobuf/io/zero_copy_stream.h"
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"



class ChatSession : public std::enable_shared_from_this<ChatSession>
{
    std::vector<google::protobuf::uint8> _bytesArray;
    char _temp;
    unsigned int _currentMessageSize;
    unsigned char * _currentMessageBuffer;
public:
    ChatSession(boost::asio::ip::tcp::socket socket)
        : _socket(std::move(socket))
    {

    }

    void start()
    {
        tryReadHeader();
    }

    void deliver(const ChatMessage::ChatMessage& msg)
    {

    }

private:
    void tryReadHeader()
    {
        auto self(shared_from_this());
        boost::asio::async_read(_socket,
                                boost::asio::buffer(&_temp, 1),
        [this, self](boost::system::error_code ec, std::size_t) {
            if (!ec) {
                _bytesArray.push_back(_temp);
                //std::cout << "Got char! " << _temp << std::endl;
                google::protobuf::io::CodedInputStream input(_bytesArray.data(),_bytesArray.size());
                google::protobuf::uint32 result;
                if(input.ReadVarint32(&result)) {
                    std::cout << "Incoming message size: " << result << std::endl;
                    _currentMessageSize = result;
                    _currentMessageBuffer = new unsigned char[result];
                    _bytesArray.clear();
                    tryReadBody();
                } else {
                    tryReadHeader();
                }
            } else {
                std::cout << "Error! " << std::endl;
            }

        });
    }

    void tryReadBody()
    {
        auto self(shared_from_this());

        boost::asio::async_read(_socket,
                                boost::asio::buffer(_currentMessageBuffer, _currentMessageSize),
        [this, self](boost::system::error_code ec, std::size_t) {
            if (!ec) {
                //std::cout << "Got body! " << std::endl;
                google::protobuf::io::CodedInputStream input(_currentMessageBuffer, _currentMessageSize);
                ChatMessage::ChatMessage msg;
                if(msg.ParseFromCodedStream(&input)) {
                    std::cout << "Message: " << msg.text() << " From: " << msg.sender() << std::endl;
                }
                tryReadHeader();
            } else {
                std::cout << "Error! " << std::endl;
            }
        });
    }

    boost::asio::ip::tcp::socket _socket;
};

#endif // CHATSESSION_H
