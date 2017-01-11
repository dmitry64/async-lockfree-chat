#ifndef CHATSUBSCRIBER_H
#define CHATSUBSCRIBER_H
#include <iostream>
#include "ChatMessage.pb.h"
#include <boost/shared_ptr.hpp>

class ChatSubscriber : public std::enable_shared_from_this<ChatSubscriber>
{
public:
    class SimpleMessage
    {
    public:
        char * text;
        char * sender;
    };
    ChatSubscriber();
    virtual ~ChatSubscriber();
    virtual void putMessage(const ChatMessage::ChatMessage &msg) = 0;
    bool isDead() const;

protected:
    bool _isDead;
};

#endif // CHATSUBSCRIBER_H
