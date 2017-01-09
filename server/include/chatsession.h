#ifndef CHATSESSION_H
#define CHATSESSION_H

#include <iostream>
#include <boost/asio.hpp>
#include "ChatMessage.pb.h"
#include "google/protobuf/io/zero_copy_stream.h"
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"

class chat_participant
{
public:
    virtual ~chat_participant() {}
    virtual void deliver(const ChatMessage::ChatMessage& msg) = 0;
};

typedef std::shared_ptr<chat_participant> chat_participant_ptr;



class ChatSession : public chat_participant,
        public std::enable_shared_from_this<ChatSession>
{
    std::vector<google::protobuf::uint8> _bytesArray;
    char _temp;
    unsigned int _currentMessageSize;
    unsigned char * _currentMessageBuffer;
public:
    ChatSession(boost::asio::ip::tcp::socket socket)
        : socket_(std::move(socket))
    {

    }

    void start()
    {
        tryReadHeader();
    }

    void deliver(const ChatMessage::ChatMessage& msg)
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
                //std::cout << "Got body! " << std::endl;
                google::protobuf::io::CodedInputStream input(_currentMessageBuffer, _currentMessageSize);
                ChatMessage::ChatMessage msg;
                if(msg.ParseFromCodedStream(&input)) {
                    std::cout << "Message: " << msg.text() << " From: " << msg.sender() << std::endl;
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

    boost::asio::ip::tcp::socket socket_;
};

#endif // CHATSESSION_H
