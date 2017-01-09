#include "chatsession.h"


ChatSession::ChatSession(boost::asio::io_service &io_service, boost::asio::ip::tcp::socket socket, ChatRoom * room)
    : _socket(std::move(socket)), _room(room), _io_service(io_service)
{
    room->subscribe(this);
    _current = 0;
}

void ChatSession::start()
{
    tryReadHeader();
}

void ChatSession::deliver(const ChatMessage::ChatMessage &msg)
{
    std::cout << "Delivering: " << msg.text() << " from: " << msg.sender() << std::endl;
}

unsigned int ChatSession::getLastReceivedMessageNumber()
{
    return _current;
}

void ChatSession::prepareToSync()
{
    std::cout << "prepareToSync" << std::endl;
    _io_service.post(
    [this]() {
        std::cout << "prepareToSync post" << std::endl;

        auto temp = _room->getMessagesFromSpecificNumber(_current);
        for(int i=0; i<temp.size(); i++) {
            deliver(temp.at(i)->second);
        }
    });
}

void ChatSession::tryReadHeader()
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
            _socket.close();
        }

    });
}

void ChatSession::tryReadBody()
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
                _room->addMessage(msg);
            }
            tryReadHeader();
        } else {
            std::cout << "Error! " << std::endl;
            _socket.close();
        }
    });
}
