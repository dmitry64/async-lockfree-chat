#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "chatserver.h"
#include <cds/init.h>
#include <cds/gc/hp.h>


class ThreadRunner // Wrapper for libcds initialization
{
    boost::asio::io_service & io_service;
public:
    ThreadRunner(boost::asio::io_service& service) : io_service(service)
    {
    }
    void start()
    {
        cds::threading::Manager::attachThread();
        io_service.run();
        cds::threading::Manager::detachThread();
    }
};

int main(int argc, char* argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION; // If it fails, set CMAKE_PREFIX_PATH=[path to protobuf] enviroment variable
    cds::Initialize();
    cds::gc::HP hpGC;
    cds::threading::Manager::attachThread();

    try {
        if (argc < 2) {
            std::cerr << "Usage: chat_server <port> [<port> ...]\n";
            return 1;
        }

        boost::asio::io_service io_service(4);
        ThreadRunner runner(io_service);
        boost::thread_group threads;
        std::list<ChatServer> servers;
        for (int i = 1; i < argc; ++i) {
            boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), std::atoi(argv[i]));
            servers.emplace_back(io_service, endpoint);
        }

        for (int i = 0; i < 4; ++i) {
            threads.create_thread(boost::bind(&ThreadRunner::start, &runner));
        }
        std::cout << "Running!" << std::endl;
        std::cin.get();
        io_service.stop();

        threads.join_all();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    cds::Terminate();
    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
