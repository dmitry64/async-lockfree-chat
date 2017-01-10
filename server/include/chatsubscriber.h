#ifndef CHATSUBSCRIBER_H
#define CHATSUBSCRIBER_H
#include <iostream>
#include "ChatMessage.pb.h"
#include <boost/lockfree/queue.hpp>

class ChatSubscriber : public std::enable_shared_from_this<ChatSubscriber>
{

public:
    class SimpleMessage{
    public:
        char * text;
        char * sender;
    };
    ChatSubscriber();
    virtual ~ChatSubscriber();
    virtual void invalidate() = 0;
    void putMessage(ChatMessage::ChatMessage &msg);
    bool isDead() const;

protected:
    bool _isDead;
    boost::lockfree::queue<SimpleMessage> * _messages;
};

#endif // CHATSUBSCRIBER_H
