#include "chatsession.h"


ChatSession::ChatSession(boost::asio::io_service &io_service, boost::asio::ip::tcp::socket socket, ChatRoom * room)
    : _socket(std::move(socket)), _room(room), _io_service(io_service)
{
    room->subscribe(this);
}

ChatSession::~ChatSession()
{
    //_room->unsubscribe(this);
}

void ChatSession::start()
{
    tryReadHeader();
}

std::pair<char *, unsigned int> ChatSession::createBufferFromMessage(const ChatMessage::ChatMessage &message)
{
    std::pair<char *, unsigned int> buffer;
    google::protobuf::uint32 message_length = message.ByteSize();
    int prefix_length = sizeof(message_length);
    int buffer_length = prefix_length + message_length;
    buffer.first = new char[buffer_length];

    google::protobuf::io::ArrayOutputStream array_output(buffer.first, buffer_length);
    google::protobuf::io::CodedOutputStream coded_output(&array_output);

    coded_output.WriteVarint32(message_length);
    message.SerializeToCodedStream(&coded_output);
    buffer.second = coded_output.ByteCount();

    return buffer;
}


void ChatSession::deliver(const SimpleMessage &msg)
{
    auto self(shared_from_this());
    if(_socket.is_open()) {
        std::cout << "Delivering: " << msg.text << " from: " << msg.sender << std::endl;
        ChatMessage::ChatMessage message;
        message.set_text(msg.text);
        message.set_sender(msg.sender);
        auto buf = createBufferFromMessage(message);
        usleep(1000);
        boost::asio::async_write(_socket,
                                 boost::asio::buffer(buf.first,
                                         buf.second),
        [this, self, msg, buf](boost::system::error_code ec, std::size_t ) {
            delete buf.first;

            if(!ec) {
                std::cout << "Send!" << std::endl;
            } else {
                std::cout << "send Error!" << std::endl;
                //_room->unsubscribe(this);
            }
        });
    }
}

void ChatSession::invalidate()
{
    std::cout << "prepareToSync" << std::endl;
    auto self(shared_from_this());
    if(!_isDead) {
        _io_service.post(
        [this, self]() {
            std::cout << "sync post" << std::endl;
            SimpleMessage msg;
            if(!_messages->empty()) {
                if(_messages->pop(msg)){
                    //std::cout << "Pop: " << msg.text << " from: " << msg.sender << std::endl;
                    deliver(msg);
                }
            }
            std::cout << "sync post end" << std::endl;
            //_valid = true;
        });
    } else {
    }
}

void ChatSession::tryReadHeader()
{
    auto self(shared_from_this());
    if(!_isDead) {
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
                std::cout << "read head Error! " << std::endl;
            }
        });
    }
}

void ChatSession::tryReadBody()
{
    auto self(shared_from_this());
    if(!_isDead) {
        boost::asio::async_read(_socket,
                                boost::asio::buffer(_currentMessageBuffer, _currentMessageSize),
        [this, self](boost::system::error_code ec, std::size_t) {
            if (!ec) {
                //std::cout << "Got body! " << std::endl;
                google::protobuf::io::CodedInputStream input(_currentMessageBuffer, _currentMessageSize);
                ChatMessage::ChatMessage msg;
                if(msg.ParseFromCodedStream(&input)) {
                    //std::cout << "Message: " << msg.text() << " From: " << msg.sender() << std::endl;
                    delete _currentMessageBuffer;
                    _room->addMessage(msg);
                }
                tryReadHeader();
            } else {
                std::cout << "read body Error! " << std::endl;
            }
        });
    }
}
