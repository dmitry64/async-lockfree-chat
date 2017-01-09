#include "chatroom.h"

ChatRoom::ChatRoom()
{
    pthread_mutex_init(&_mutex, NULL);
}

void ChatRoom::addMessage(ChatMessage::ChatMessage &msg)
{
    pthread_mutex_lock(&_mutex);
    std::pair<unsigned int, ChatMessage::ChatMessage> pair;
    pair.first = _messages.size() + 1;
    pair.second = msg;
    _messages.push_back(pair);

    for(int i = 0; i<_subscribers.size(); i++) {
        int temp = _subscribers.at(i)->getLastReceivedMessageNumber();
        if(temp < _messages.size()) {
            _subscribers.at(i)->prepareToSync();
        }
    }
    pthread_mutex_unlock(&_mutex);
}

void ChatRoom::subscribe(ChatSubscriber *sub)
{
    _subscribers.push_back(sub);
}

std::vector<std::pair<unsigned int, ChatMessage::ChatMessage> *> ChatRoom::getMessagesFromSpecificNumber(unsigned int & num)
{
    if(num < _messages.size()) {
        std::vector<std::pair<unsigned int, ChatMessage::ChatMessage> *> temp;
        for(int i = num; i<_messages.size(); i++) {
            temp.push_back(&(_messages.at(i)));
        }
        num = _messages.size();
        return temp;
    } else {
        return std::vector<std::pair<unsigned int, ChatMessage::ChatMessage> *>();
    }
}
