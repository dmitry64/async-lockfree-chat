#include "chatroom.h"

ChatRoom::ChatRoom()
{
}

ChatRoom::~ChatRoom()
{
}

void ChatRoom::addMessage(ChatMessage::ChatMessage &msg)
{
    for(auto it=_list.begin(); it!=_list.end(); it.operator ++()) {
        it.operator *()->putMessage(msg);
    }
}

void ChatRoom::subscribe(ChatSubscriber *sub)
{
    _list.insert(sub);
}

void ChatRoom::unsubscribe(ChatSubscriber *sub)
{
    _list.erase(sub);
}
