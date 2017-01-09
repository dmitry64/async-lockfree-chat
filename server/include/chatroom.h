#ifndef CHATROOM_H
#define CHATROOM_H
#include <string>
#include <vector>
#include <deque>
#include <mutex>
#include "chatsubscriber.h"
#include "ChatMessage.pb.h"

class ChatRoom
{
    pthread_mutex_t _mutex;
    std::vector< std::pair<unsigned int, ChatMessage::ChatMessage> > _messages;
    std::deque< ChatSubscriber* > _subscribers;
public:
    ChatRoom();
    void addMessage(ChatMessage::ChatMessage & msg);
    void subscribe(ChatSubscriber* sub);
    std::vector< std::pair<unsigned int, ChatMessage::ChatMessage> * > getMessagesFromSpecificNumber(unsigned int &num);
};

#endif // CHATROOM_H
