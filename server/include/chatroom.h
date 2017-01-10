#ifndef CHATROOM_H
#define CHATROOM_H
#include <string>
#include <vector>
#include <deque>
//#include <mutex>
#include <boost/shared_ptr.hpp>
#include "chatsubscriber.h"
#include "ChatMessage.pb.h"
#include <boost/thread/shared_mutex.hpp>

class ChatRoom
{
    //pthread_mutex_t _mutex;
    //boost::lockfree::stack< std::pair<unsigned int, ChatMessage::ChatMessage> > _messages;
    //std::vector< std::pair<unsigned int, ChatMessage::ChatMessage> > _messages;
    std::deque<ChatSubscriber* > _subscribers;
    mutable boost::shared_mutex _connectionsMuntex;
public:
    ChatRoom();
    ~ChatRoom();
    void addMessage(ChatMessage::ChatMessage & msg);
    void subscribe(ChatSubscriber* sub);
    void unsubscribe(ChatSubscriber* sub);
};

#endif // CHATROOM_H
