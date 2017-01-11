#ifndef CHATSESSION_H
#define CHATSESSION_H

#include <iostream>
#include <boost/asio.hpp>
#include "chatsubscriber.h"
#include "chatroom.h"
#include "google/protobuf/io/zero_copy_stream.h"
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"

class ChatSession : public ChatSubscriber
{
    std::vector<google::protobuf::uint8> _headerBuffer;
    std::vector<google::protobuf::uint8> _bodyBuffer;
    char _temp;
    unsigned int _currentMessageSize;
    unsigned char * _currentMessageBuffer;

    boost::asio::io_service& _io_service;
public:
    ChatSession(boost::asio::io_service& io_service, boost::asio::ip::tcp::socket socket, ChatRoom * room);
    ~ChatSession();
    void start();
    void putMessage(const ChatMessage::ChatMessage &msg);
    std::vector<google::protobuf::uint8> createBufferFromMessage(const ChatMessage::ChatMessage &message);
private:
    void tryReadHeader();
    void tryReadBody();
    boost::asio::ip::tcp::socket _socket;
    ChatRoom * _room;
};

#endif // CHATSESSION_H
