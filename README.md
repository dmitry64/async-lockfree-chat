# Asynchronous lock-free high performance chat
This is an example of a high performance chat written in C++. Here are some features:
* Asynchronous communication is handled by Boost.Asio in a scaleable thread pool
* Messages are encoded with variable-length encoding using Google Protocol Buffers
* Lock-free syncronization is implemented using libcds containers

# How to build
```sh
git clone --recursive git://github.com/dmitry64/async-lockfree-chat.git
mkdir build
cd build
cmake ../async-lockfree-chat
make -j [number of cpu cores + 1]
```
# How to run
Run server:
```sh
cd build/server
./server -p 1234 -t 4
```
Run clients:
```sh
cd build/client
./client -p 1234 -a localhost -n Batman
```

# References
* Concurrent Data Structures library - [libcds](https://github.com/khizmax/libcds)
* Boost.Asio - [Basic overview](http://www.boost.org/doc/libs/1_62_0/doc/html/boost_asio/overview/core/basics.html)
* Google Protocol Buffers [Developer guide](https://developers.google.com/protocol-buffers)
