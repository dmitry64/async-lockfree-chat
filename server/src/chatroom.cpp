#include "chatroom.h"

ChatRoom::ChatRoom()
{

}

void ChatRoom::addMessage(ChatMessage::ChatMessage &msg)
{
    for(auto it=_subscribers.begin(); it!=_subscribers.end(); it++) {
        auto tempPtr = it.operator *();
        if(!(tempPtr->isDead())) {
            tempPtr->putMessage(msg);
            //tempPtr->invalidate();
        } else {
            _subscribers.erase(it);
        }
    }
}

void ChatRoom::subscribe(ChatSubscriber *sub)
{
    _subscribers.push_back(sub);
}

/*void ChatRoom::unsubscribe(ChatSubscriber *sub)
{

}*/

