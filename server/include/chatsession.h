#ifndef CHATSESSION_H
#define CHATSESSION_H

#include <iostream>
#include <boost/asio.hpp>
#include "chatroom.h"
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
    ChatSession(boost::asio::ip::tcp::socket socket, ChatRoom * room);

    void start();
    void deliver(const ChatMessage::ChatMessage& msg);
private:
    void tryReadHeader();
    void tryReadBody();
    boost::asio::ip::tcp::socket _socket;
    ChatRoom * _room;
};

#endif // CHATSESSION_H
