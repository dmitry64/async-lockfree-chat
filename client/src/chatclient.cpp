#include "chatclient.h"

std::pair<char *, unsigned int> ChatClient::createBufferFromMessage(const ChatMessage::ChatMessage &message)
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

void ChatClient::write(const ChatMessage::ChatMessage &msg)
{
    std::cout << "write" << std::endl;
    auto pair = createBufferFromMessage(msg);
    _encodedMessages.push_back(pair);
    io_service_.post(
    [this, msg]() {
        std::cout << "post" << std::endl;
        do_write();
    });
}

void ChatClient::do_write()
{
    std::cout << "do_write!" << std::endl;
    if(!_encodedMessages.empty()) {
        boost::asio::async_write(socket_,
                                 boost::asio::buffer(_encodedMessages.front().first,
                                         _encodedMessages.front().second),
        [this](boost::system::error_code ec, std::size_t ) {
            delete _encodedMessages.front().first;
            _encodedMessages.pop_front();
            if(!ec) {
                std::cout << "Send!" << std::endl;
            } else {
                std::cout << "Error!" << std::endl;
            }
        });
    }
}

void ChatClient::close()
{
    io_service_.post([this]() {
        socket_.close();
    });
}
