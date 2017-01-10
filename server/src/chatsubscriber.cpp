#include "chatsubscriber.h"

ChatSubscriber::ChatSubscriber()
{
    _messages = new boost::lockfree::queue<SimpleMessage>(30);
    _isDead = false;
}

ChatSubscriber::~ChatSubscriber()
{
    std::cout << "destroying chat subscriber" << std::endl;
    _isDead = true;
}

void ChatSubscriber::putMessage(ChatMessage::ChatMessage &msg)
{
    std::cout << "putting message!" << std::endl;
    auto self(shared_from_this());
    SimpleMessage sm;
    const char * oldSender = msg.sender().c_str();
    const char * oldText = msg.text().c_str();
    sm.sender = new char[strlen(oldSender)];
    sm.text = new char[strlen(oldText)];
    strcpy(sm.sender,oldSender);
    strcpy(sm.text,oldText);
    //std::cout << "size: " << strlen(oldText) << "putMessage: " << sm.text << " from: " << sm.sender << std::endl;
    _messages->push(sm);
    invalidate();
}

bool ChatSubscriber::isDead() const
{
    return _isDead;
}
