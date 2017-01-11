#include <cstdlib>
#include <deque>
#include <iostream>
#include <iomanip>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "chatserver.h"
#include <cds/init.h>
#include <cds/gc/hp.h>

#define DEFAULT_THREAD_COUNT 2
#define DEFAULT_PORT 1234

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

    unsigned int threadCount = DEFAULT_THREAD_COUNT;
    unsigned int port = DEFAULT_PORT;
    int opt = 0;

    while ((opt = getopt (argc, argv, "p:t:h")) != -1) {
        switch(opt) {
        case 'p':
            port = std::atoi(optarg);
            break;
        case 't':
            threadCount = std::atoi(optarg);
            break;
        default:
        case 'h':
            std::cout << "Usage: server [OPTION]..." << std::endl << std::endl;
            std::cout << std::setw(3) << "-p" << "\t" << "Set server port" << std::endl;
            std::cout << std::setw(3) << "-t" << "\t" << "Set server thread count" << std::endl;
            std::cout << std::endl;
            std::cout << "Example: server -p 1234 -t 4"<< std::endl;
            std::cout << "Starts a server on port 1234 with 4 threads." << std::endl;
            std::cout << std::endl;
            std::cout << "Author: Zykov Dmitry" << std::endl;
            return 0;
        }
    }

    cds::Initialize();
    cds::gc::HP hpGC; // Initializing hazard pointers gc singleton
    cds::threading::Manager::attachThread();

    try {
        boost::asio::io_service io_service(threadCount);
        ThreadRunner runner(io_service);
        boost::thread_group threads;

        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);
        ChatServer server(io_service,endpoint);

        for (int i = 0; i < threadCount; ++i) {
            threads.create_thread(boost::bind(&ThreadRunner::start, &runner));
        }

        std::cout << "Server started..." << std::endl;
        std::cin.get();
        std::cout << "Shutdown..." << std::endl;
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
