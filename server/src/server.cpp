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

    unsigned int threadCount = 1;
    unsigned int port = 1234;

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
