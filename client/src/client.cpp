#include <cstdlib>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include "chatclient.h"
#include "ChatMessage.pb.h"

int main(int argc, char* argv[])
{
    try {
        if (argc != 3) {
            std::cerr << "Usage: chat_client <host> <port>\n";
            return 1;
        }

        boost::asio::io_service io_service;
        boost::asio::io_service::work work(io_service);

        boost::asio::ip::tcp::resolver resolver(io_service);
        auto endpoint_iterator = resolver.resolve({ argv[1], argv[2] });
        ChatClient c(io_service, endpoint_iterator);

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
