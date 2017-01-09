#ifndef CHATSESSION_H
#define CHATSESSION_H

#include <iostream>
#include <boost/asio.hpp>
#include "chatsubscriber.h"
#include "chatroom.h"
#include "ChatMessage.pb.h"
#include "google/protobuf/io/zero_copy_stream.h"
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"

class ChatSession : public std::enable_shared_from_this<ChatSession>, public ChatSubscriber
{
    std::vector<google::protobuf::uint8> _bytesArray;
    unsigned int _current;
    char _temp;
    unsigned int _currentMessageSize;
    unsigned char * _currentMessageBuffer;
    boost::asio::io_service& _io_service;
public:
    ChatSession(boost::asio::io_service& io_service, boost::asio::ip::tcp::socket socket, ChatRoom * room);
    void start();
    void deliver(const ChatMessage::ChatMessage& msg);

    unsigned int getLastReceivedMessageNumber();
    void prepareToSync();
private:
    void tryReadHeader();
    void tryReadBody();
    boost::asio::ip::tcp::socket _socket;
    ChatRoom * _room;
};

#endif // CHATSESSION_H
