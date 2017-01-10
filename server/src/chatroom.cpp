#include "chatroom.h"

ChatRoom::ChatRoom()
{

}

ChatRoom::~ChatRoom()
{
    boost::unique_lock< boost::shared_mutex > lock(_connectionsMuntex);
    _subscribers.clear();
}

void ChatRoom::addMessage(ChatMessage::ChatMessage &msg)
{
    boost::shared_lock<boost::shared_mutex> lock(_connectionsMuntex);
    for(auto it=_subscribers.begin(); it!=_subscribers.end(); it++) {
        it.operator *()->putMessage(msg);
    }
    lock.unlock();
}

void ChatRoom::subscribe(ChatSubscriber *sub)
{
    boost::unique_lock< boost::shared_mutex > lock(_connectionsMuntex);
    _subscribers.push_back(sub);
    lock.unlock();
}

void ChatRoom::unsubscribe(ChatSubscriber *sub)
{
    boost::unique_lock< boost::shared_mutex > lock(_connectionsMuntex);
    std::cout << "Unsubscribing" << std::endl;
    for(auto it=_subscribers.begin(); it!=_subscribers.end(); it++) {
        auto cur = it.operator *();
        if(cur == sub){
            _subscribers.erase(it);
            break;
        }
    }
    lock.unlock();
}

