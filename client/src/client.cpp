#include <cstdlib>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include "chatclient.h"
#include "ChatMessage.pb.h"

#define DEFAULT_CLIENT_NAME "Anonymous"
#define DEFAULT_SERVER_ADDRESS "localhost"
#define DEFAULT_PORT "1234"

enum ClientMode {
    normal = 0,
    spammer = 1
};

int main(int argc, char* argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    ClientMode mode = normal;
    std::string clientName = DEFAULT_CLIENT_NAME;
    std::string serverAddress = DEFAULT_SERVER_ADDRESS;
    std::string portStr = DEFAULT_PORT;

    int opt = 0;

    while ((opt = getopt (argc, argv, "p:a:n:sh")) != -1) {
        switch(opt) {
        case 'p':
            portStr = std::string(optarg);
            break;
        case 'a':
            serverAddress = std::string(optarg);
            break;
        case 'n':
            clientName = std::string(optarg);
            break;
        case 's':
            mode = spammer;
            break;
        default:
        case 'h':
            std::cout << "Usage: client [OPTION]..." << std::endl << std::endl;
            std::cout << std::setw(3) << "-p" << "\t" << "Set server port" << std::endl;
            std::cout << std::setw(3) << "-a" << "\t" << "Set server address" << std::endl;
            std::cout << std::setw(3) << "-n" << "\t" << "Set client name" << std::endl;
            std::cout << std::setw(3) << "-s" << "\t" << "Sets client spammer mode" << std::endl;
            std::cout << std::endl;
            std::cout << "Example: client -p 1234 -a localhost -n Batman"<< std::endl;
            std::cout << "Connects to server on port localhost:1234 with name 'Batman'." << std::endl;
            std::cout << std::endl;
            std::cout << "Author: Zykov Dmitry" << std::endl;
            return 0;
        }
    }

    try {
        boost::asio::io_service io_service;
        boost::asio::io_service::work work(io_service);

        boost::asio::ip::tcp::resolver resolver(io_service);
        auto endpoint_iterator = resolver.resolve({ serverAddress.c_str(), portStr.c_str() });
        ChatClient c(io_service, endpoint_iterator);

        std::thread t([&io_service]() {
            io_service.run();
        });

        std::string temp;

        switch(mode) {
        case normal:
            while(true) {
                std::cin >> temp;
                ChatMessage::ChatMessage message;
                message.set_sender(clientName);
                message.set_text(temp);
                c.write(message);
            }
            break;
        case spammer:
            while(true) {
                usleep(100);
                ChatMessage::ChatMessage message;
                message.set_sender(clientName);
                message.set_text("Random text #" + boost::lexical_cast<std::string>(rand()%1000) + " spam!");
                c.write(message);
            }
            break;
        }

        c.close();
        t.join();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
