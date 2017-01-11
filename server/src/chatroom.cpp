#include "chatroom.h"

ChatRoom::ChatRoom()
{
}

ChatRoom::~ChatRoom()
{
}

void ChatRoom::addMessage(ChatMessage::ChatMessage &msg)
{
    for(auto it=_connectedClientsList.begin(); it!=_connectedClientsList.end(); it.operator ++()) {
        it.operator *()->putMessage(msg);
    }
}

void ChatRoom::subscribe(ChatSubscriber *sub)
{
    _connectedClientsList.insert(sub);
}

void ChatRoom::unsubscribe(ChatSubscriber *sub)
{
    _connectedClientsList.erase(sub);
}
