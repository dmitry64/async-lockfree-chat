#include "chatclient.h"

std::vector<google::protobuf::uint8> ChatClient::createBufferFromMessage(const ChatMessage::ChatMessage &message)
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

void ChatClient::write(const ChatMessage::ChatMessage &msg)
{
    auto buf = createBufferFromMessage(msg);
    boost::asio::async_write(_socket,
                             boost::asio::buffer(buf.data(), buf.size()),
    [this](boost::system::error_code ec, std::size_t ) {
        if(ec) {
            std::cout << "Error!" << ec.message() << std::endl;
        }
    });
}

void ChatClient::tryReadHeader()
{
    if(_socket.is_open()) {
        boost::asio::async_read(_socket,
                                boost::asio::buffer(&_temp, 1),
        [this](boost::system::error_code ec, std::size_t) {
            if (!ec) {
                _headerBuffer.push_back(_temp);
                google::protobuf::io::CodedInputStream input(_headerBuffer.data(),_headerBuffer.size());
                google::protobuf::uint32 result;
                if(input.ReadVarint32(&result)) {
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

void ChatClient::tryReadBody()
{
    if(_socket.is_open()) {
        boost::asio::async_read(_socket,
                                boost::asio::buffer(_bodyBuffer.data(), _bodyBuffer.size()),
        [this](boost::system::error_code ec, std::size_t) {
            if (!ec) {
                google::protobuf::io::CodedInputStream input(_bodyBuffer.data(), _bodyBuffer.size());
                ChatMessage::ChatMessage msg;
                if(msg.ParseFromCodedStream(&input)) {
                    std::cout << std::setw(16) << msg.sender() <<": " << msg.text() << std::endl;
                }
                _bodyBuffer.clear();
                tryReadHeader();
            }
        });
    }
}

void ChatClient::close()
{
    io_service_.post([this]() {
        _socket.close();
    });
}
