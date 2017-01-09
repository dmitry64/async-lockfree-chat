#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include "ChatMessage.pb.h"
#include "google/protobuf/io/zero_copy_stream.h"
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"

using boost::asio::ip::tcp;

class chat_client
{
    std::deque< std::pair<char *, unsigned int> > _encodedMessages;
public:
    chat_client(boost::asio::io_service& io_service,
                tcp::resolver::iterator endpoint_iterator)
        : io_service_(io_service),
          socket_(io_service)
    {
        ChatMessage::ChatMessage message;
        message.set_sender("Sender");
        message.set_text("Text here");
        auto pair = createBufferFromMessage(message);
        _encodedMessages.push_back(pair);

        do_connect(endpoint_iterator);
    }

    std::pair<char *, unsigned int> createBufferFromMessage(const ChatMessage::ChatMessage & message)
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


    void write(const ChatMessage::ChatMessage& msg)
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

    void close()
    {
        io_service_.post([this]() {
            socket_.close();
        });
    }

private:
    void do_connect(tcp::resolver::iterator endpoint_iterator)
    {
        boost::asio::async_connect(socket_, endpoint_iterator,
        [this](boost::system::error_code ec, tcp::resolver::iterator) {
            if (!ec) {
                do_write();
            }
        });
    }

    /* void do_read_header()
     {
       boost::asio::async_read(socket_,
           boost::asio::buffer(read_msg_.data(), chat_message::header_length),
           [this](boost::system::error_code ec, std::size_t )
           {
             if (!ec && read_msg_.decode_header())
             {
               do_read_body();
             }
             else
             {
               socket_.close();
             }
           });
     }

     void do_read_body()
     {
       boost::asio::async_read(socket_,
           boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
           [this](boost::system::error_code ec, std::size_t)
           {
             if (!ec)
             {
               std::cout.write(read_msg_.body(), read_msg_.body_length());
               std::cout << "\n";
               do_read_header();
             }
             else
             {
               socket_.close();
             }
           });
     }*/

    void do_write()
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

private:
    boost::asio::io_service& io_service_;
    tcp::socket socket_;
};

int main(int argc, char* argv[])
{
    try {
        if (argc != 3) {
            std::cerr << "Usage: chat_client <host> <port>\n";
            return 1;
        }

        boost::asio::io_service io_service;
        boost::asio::io_service::work work(io_service);

        tcp::resolver resolver(io_service);
        auto endpoint_iterator = resolver.resolve({ argv[1], argv[2] });
        chat_client c(io_service, endpoint_iterator);

        std::thread t([&io_service]() {
            io_service.run();
        });

        char line[128];
        while (std::cin.getline(line, 128)) {
            std::cout << "new line" << std::endl;
            ChatMessage::ChatMessage message;
            message.set_sender("Sender");
            message.set_text(line);
            c.write(message);
        }

        c.close();
        t.join();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
