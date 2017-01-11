#include "chatsession.h"


ChatSession::ChatSession(boost::asio::io_service &io_service, boost::asio::ip::tcp::socket socket, ChatRoom * room)
    : _socket(std::move(socket)), _room(room), _io_service(io_service), _currentMessageBuffer(0), _currentMessageSize(0), _temp(0)
{
    room->subscribe(this);
}

ChatSession::~ChatSession()
{
    _room->unsubscribe(this);
    _socket.cancel();
    _socket.close();
    std::cout << "User disconnected" << std::endl;
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

void ChatSession::putMessage(const ChatMessage::ChatMessage &msg)
{
    if(!_isDead && _socket.is_open()) {
        auto buf = createBufferFromMessage(msg);

        boost::asio::async_write(_socket,
                                 boost::asio::buffer(buf.first, buf.second),
        [this, msg, buf](boost::system::error_code ec, std::size_t ) {
            delete[] buf.first;
        });
    }
}

void ChatSession::tryReadHeader()
{
    auto self(shared_from_this());
    if(!_isDead && _socket.is_open()) {
        boost::asio::async_read(_socket,
                                boost::asio::buffer(&_temp, 1),
        [this, self](boost::system::error_code ec, std::size_t) {
            if (!ec) {
                _bytesArray.push_back(_temp);
                google::protobuf::io::CodedInputStream input(_bytesArray.data(),_bytesArray.size());
                google::protobuf::uint32 result;
                if(input.ReadVarint32(&result)) {
                    _currentMessageSize = result;
                    _currentMessageBuffer = new unsigned char[result];
                    _bytesArray.clear();
                    tryReadBody();
                }
                else {
                    tryReadHeader();
                }
            }
        });
    }
}

void ChatSession::tryReadBody()
{
    auto self(shared_from_this());
    if(!_isDead && _socket.is_open()) {
        boost::asio::async_read(_socket,
                                boost::asio::buffer(_currentMessageBuffer, _currentMessageSize),
        [this, self](boost::system::error_code ec, std::size_t) {
            if (!ec) {
                google::protobuf::io::CodedInputStream input(_currentMessageBuffer, _currentMessageSize);
                ChatMessage::ChatMessage msg;
                if(msg.ParseFromCodedStream(&input)) {
                    std::cout << "Message: " << msg.text() << " From: " << msg.sender() << std::endl;
                    delete[] _currentMessageBuffer;
                    _room->addMessage(msg);
                }
                tryReadHeader();
            }
            else {
                delete[] _currentMessageBuffer;
            }
        });
    }
}
