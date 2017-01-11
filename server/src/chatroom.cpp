#include "chatroom.h"

ChatRoom::ChatRoom()
{
    //_list.insert(1);
    //auto it = _list.begin();
}

ChatRoom::~ChatRoom()
{
    //boost::unique_lock< boost::shared_mutex > lock(_connectionsMuntex);
    //_subscribers.clear();
}

void ChatRoom::addMessage(ChatMessage::ChatMessage &msg)
{
    //boost::shared_lock<boost::shared_mutex> lock(_connectionsMuntex);
    /*
    for(auto it=_subscribers.begin(); it!=_subscribers.end(); it++) {
        it.operator *()->putMessage(msg);
    }*/

    for(auto it=_list.begin(); it!=_list.end(); it.operator ++()) {

        it.operator *()->putMessage(msg);
    }
    //auto it = _list.begin();

    //lock.unlock();
}

void ChatRoom::subscribe(ChatSubscriber *sub)
{
    //boost::unique_lock< boost::shared_mutex > lock(_connectionsMuntex);
    //_subscribers.push_back(sub);
    //_list.insert()
    std::cout << "qwe" << std::endl;

    _list.insert(sub);
    std::cout << "asd" << std::endl;
    //lock.unlock();
}

void ChatRoom::unsubscribe(ChatSubscriber *sub)
{
    //boost::unique_lock< boost::shared_mutex > lock(_connectionsMuntex);

    /*
        for(auto it=_subscribers.begin(); it!=_subscribers.end(); it++) {
            auto cur = it.operator *();
            if(cur == sub) {
                _subscribers.erase(it);
                break;
            }
        }*/
    _list.erase(sub);
    //lock.unlock();
}
