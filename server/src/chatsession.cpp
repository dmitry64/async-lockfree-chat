#include "chatsession.h"


ChatSession::ChatSession(boost::asio::io_service &io_service, boost::asio::ip::tcp::socket socket, ChatRoom * room)
    : _socket(std::move(socket)), _room(room), _io_service(io_service), _temp(0)
{
    room->subscribe(this);
    std::cout << "Client connected" << std::endl;
}

ChatSession::~ChatSession()
{
    _room->unsubscribe(this);
    _socket.cancel();
    _socket.close();
    std::cout << "Client disconnected" << std::endl;
}

void ChatSession::start()
{
    tryReadHeader();
}

std::vector<google::protobuf::uint8> ChatSession::createBufferFromMessage(const ChatMessage::ChatMessage &message)
{
    std::vector<google::protobuf::uint8> buffer;

    google::protobuf::uint32 messageLength = message.ByteSize();
    int prefixLength = sizeof(messageLength);
    int maxBufferLength = prefixLength + messageLength;
    buffer.resize(maxBufferLength);

    google::protobuf::io::ArrayOutputStream array_output(buffer.data(), buffer.size());
    google::protobuf::io::CodedOutputStream coded_output(&array_output);

    coded_output.WriteVarint32(messageLength);
    message.SerializeToCodedStream(&coded_output);
    buffer.resize(coded_output.ByteCount());

    return buffer;
}

void ChatSession::putMessage(const ChatMessage::ChatMessage &msg)
{
    if(!_isDead && _socket.is_open()) {
        auto buf = createBufferFromMessage(msg);

        boost::asio::async_write(_socket,
                                 boost::asio::buffer(buf.data(), buf.size()),
        [this, buf](boost::system::error_code ec, std::size_t ) {});
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
                _headerBuffer.push_back(_temp);
                google::protobuf::io::CodedInputStream input(_headerBuffer.data(),_headerBuffer.size());
                google::protobuf::uint32 result;
                if(input.ReadVarint32(&result)) {
                    _bodyBuffer.clear();
                    _bodyBuffer.resize(result);
                    _headerBuffer.clear();
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
                                boost::asio::buffer(_bodyBuffer.data(), _bodyBuffer.size()),
        [this, self](boost::system::error_code ec, std::size_t) {
            if (!ec) {
                google::protobuf::io::CodedInputStream input(_bodyBuffer.data(), _bodyBuffer.size());
                ChatMessage::ChatMessage msg;
                if(msg.ParseFromCodedStream(&input)) {
                    std::cout << "Message: " << msg.text() << " From: " << msg.sender() << std::endl;
                    _room->addMessage(msg);
                }
                else {
                    std::cout << "Garbage: " << _bodyBuffer.size()<< std::endl;
                }
                _bodyBuffer.clear();
                tryReadHeader();
            }
        });
    }
}
