#include "chatsubscriber.h"

unsigned int ChatSubscriber::id() const
{
    return _id;
}

void ChatSubscriber::setId(unsigned int id)
{
    _id = id;
}

ChatSubscriber::ChatSubscriber()
{
    //_messages = new boost::lockfree::queue<SimpleMessage>(30);
    _isDead = false;
}

ChatSubscriber::~ChatSubscriber()
{
    std::cout << "destroying chat subscriber" << std::endl;
    _isDead = true;
}

bool ChatSubscriber::isDead() const
{
    return _isDead;
}
