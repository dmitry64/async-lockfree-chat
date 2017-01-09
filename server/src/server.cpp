#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <boost/asio.hpp>
#include "chat_message.hpp"
#include "ChatMessage.pb.h"
#include "google/protobuf/io/zero_copy_stream.h"
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"

using boost::asio::ip::tcp;

//----------------------------------------------------------------------

typedef std::deque<chat_message> chat_message_queue;

//----------------------------------------------------------------------

class chat_participant
{
public:
    virtual ~chat_participant() {}
    virtual void deliver(const chat_message& msg) = 0;
};

typedef std::shared_ptr<chat_participant> chat_participant_ptr;

//----------------------------------------------------------------------

class chat_room
{
public:
    void join(chat_participant_ptr participant)
    {
        participants_.insert(participant);
        for (auto msg: recent_msgs_)
            participant->deliver(msg);
    }

    void leave(chat_participant_ptr participant)
    {
        participants_.erase(participant);
    }

    void deliver(const chat_message& msg)
    {
        recent_msgs_.push_back(msg);
        while (recent_msgs_.size() > max_recent_msgs)
            recent_msgs_.pop_front();

        for (auto participant: participants_)
            participant->deliver(msg);
    }

private:
    std::set<chat_participant_ptr> participants_;
    enum { max_recent_msgs = 100 };
    chat_message_queue recent_msgs_;
};

//----------------------------------------------------------------------

class chat_session
    : public chat_participant,
      public std::enable_shared_from_this<chat_session>
{
    std::vector<google::protobuf::uint8> _bytesArray;
    char _temp;
    unsigned int _currentMessageSize;
    unsigned char * _currentMessageBuffer;
public:
    chat_session(tcp::socket socket, chat_room& room)
        : socket_(std::move(socket)),
          room_(room)
    {

    }

    void start()
    {
        room_.join(shared_from_this());

        tryReadHeader();
    }

    void deliver(const chat_message& msg)
    {
        //bool write_in_progress = !write_msgs_.empty();
        //write_msgs_.push_back(msg);
        //if (!write_in_progress)
        //{
        //do_write();
        //}
    }

private:
    void tryReadHeader()
    {
        auto self(shared_from_this());
        boost::asio::async_read(socket_,
                                boost::asio::buffer(&_temp, 1),
        [this, self](boost::system::error_code ec, std::size_t) {
            if (!ec) {
                _bytesArray.push_back(_temp);
                std::cout << "Got char! " << _temp << std::endl;
                google::protobuf::io::CodedInputStream input(_bytesArray.data(),_bytesArray.size());
                google::protobuf::uint32 result;
                if(input.ReadVarint32(&result)) {
                    std::cout << "Parsed unit! " << result << std::endl;
                    _currentMessageSize = result;
                    _currentMessageBuffer = new unsigned char[result];
                    _bytesArray.clear();
                    tryReadBody();
                } else {
                    tryReadHeader();
                }
            } else {
                std::cout << "Error! " << std::endl;
            }

        });
    }

    void tryReadBody()
    {
        auto self(shared_from_this());

        boost::asio::async_read(socket_,
                                boost::asio::buffer(_currentMessageBuffer, _currentMessageSize),
        [this, self](boost::system::error_code ec, std::size_t) {
            if (!ec) {
                std::cout << "Got body! " << std::endl;
                google::protobuf::io::CodedInputStream input(_currentMessageBuffer, _currentMessageSize);
                ChatMessage::ChatMessage msg;
                if(msg.ParseFromCodedStream(&input)) {
                    std::cout << "Parsed body! " << msg.text() << " From: " << msg.sender() << std::endl;
                }
                tryReadHeader();
            } else {
                std::cout << "Error! " << std::endl;
            }
        });
    }

    /*  void do_read_header()
      {
        auto self(shared_from_this());
        boost::asio::async_read(socket_,
            boost::asio::buffer(read_msg_.data(), chat_message::header_length),
            [this, self](boost::system::error_code ec, std::size_t)
            {
              if (!ec && read_msg_.decode_header())
              {
                do_read_body();
              }
              else
              {
                room_.leave(shared_from_this());
              }
            });
      }

      void do_read_body()
      {
        auto self(shared_from_this());
        boost::asio::async_read(socket_,
            boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
            [this, self](boost::system::error_code ec, std::size_t)
            {
              if (!ec)
              {
                room_.deliver(read_msg_);
                do_read_header();
              }
              else
              {
                room_.leave(shared_from_this());
              }
            });
      }

      void do_write()
      {
        auto self(shared_from_this());
        boost::asio::async_write(socket_,
            boost::asio::buffer(write_msgs_.front().data(),
              write_msgs_.front().length()),
            [this, self](boost::system::error_code ec, std::size_t )
            {
              if (!ec)
              {
                write_msgs_.pop_front();
                if (!write_msgs_.empty())
                {
                  do_write();
                }
              }
              else
              {
                room_.leave(shared_from_this());
              }
            });
      } */

    tcp::socket socket_;
    chat_room& room_;
    //chat_message read_msg_;
    //chat_message_queue write_msgs_;
};

//----------------------------------------------------------------------

class chat_server
{
public:
    chat_server(boost::asio::io_service& io_service,
                const tcp::endpoint& endpoint)
        : acceptor_(io_service, endpoint),
          socket_(io_service)
    {
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept(socket_,
        [this](boost::system::error_code ec) {
            if (!ec) {
                std::make_shared<chat_session>(std::move(socket_), room_)->start();
            }

            do_accept();
        });
    }

    tcp::acceptor acceptor_;
    tcp::socket socket_;
    chat_room room_;
};

//----------------------------------------------------------------------

int main(int argc, char* argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION; // If it fails, set CMAKE_PREFIX_PATH=[path to protobuf] enviroment variable
    try {
        if (argc < 2) {
            std::cerr << "Usage: chat_server <port> [<port> ...]\n";
            return 1;
        }

        boost::asio::io_service io_service;

        std::list<chat_server> servers;
        for (int i = 1; i < argc; ++i) {
            tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[i]));
            servers.emplace_back(io_service, endpoint);
        }

        io_service.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
