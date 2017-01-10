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
    //std::cout << "write" << std::endl;
    //auto pair = createBufferFromMessage(msg);
    //_encodedMessages.push_back(pair);
    io_service_.post(
    [this, msg]() {
        //std::cout << "post" << std::endl;
        do_write();
    });
}

void ChatClient::do_write()
{
    //std::cout << "do_write!" << std::endl;
    //if(!_encodedMessages.empty()) {
    usleep(100);
        boost::asio::async_write(_socket,
                                 boost::asio::buffer(test.first,
                                         test.second),
        [this](boost::system::error_code ec, std::size_t ) {
            //delete _encodedMessages.front().first;

            if(!ec) {

               // _encodedMessages.pop_front();
                //std::cout << "Send!" << std::endl;
            } else {
                std::cout << "Error!" << std::endl;
            }
            do_write();
        });
    //}
}

void ChatClient::tryReadHeader()
{
    boost::asio::async_read(_socket,
                            boost::asio::buffer(&_temp, 1),
    [this](boost::system::error_code ec, std::size_t) {
        if (!ec) {
            _bytesArray.push_back(_temp);
            //std::cout << "Got char! " << _temp << std::endl;
            google::protobuf::io::CodedInputStream input(_bytesArray.data(),_bytesArray.size());
            google::protobuf::uint32 result;
            if(input.ReadVarint32(&result)) {
                //std::cout << "Incoming message size: " << result << std::endl;
                _currentMessageSize = result;
                _currentMessageBuffer = new unsigned char[result];
                _bytesArray.clear();
                tryReadBody();
            } else {
                tryReadHeader();
            }
        } else {
            std::cout << "read head Error! " << std::endl;
            _socket.close();
        }

    });
}

void ChatClient::tryReadBody()
{
    boost::asio::async_read(_socket,
                            boost::asio::buffer(_currentMessageBuffer, _currentMessageSize),
    [this](boost::system::error_code ec, std::size_t) {
        if (!ec) {
            //std::cout << "Got body! " << std::endl;
            google::protobuf::io::CodedInputStream input(_currentMessageBuffer, _currentMessageSize);
            ChatMessage::ChatMessage msg;
            if(msg.ParseFromCodedStream(&input)) {
                //std::cout << "Message: " << msg.text() << " From: " << msg.sender() << std::endl;

            }
            tryReadHeader();
        } else {
            std::cout << "read body Error! " << std::endl;
            _socket.close();
        }
    });
}

void ChatClient::close()
{
    io_service_.post([this]() {
        _socket.close();
    });
}
