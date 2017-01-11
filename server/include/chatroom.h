#ifndef CHATROOM_H
#define CHATROOM_H
#include <string>
#include <vector>
#include <deque>
#include <boost/shared_ptr.hpp>
#include "chatsubscriber.h"
#include "ChatMessage.pb.h"
#include <cds/container/iterable_list_hp.h>

struct StupidPointerComparator {
    int operator ()(ChatSubscriber* v1,ChatSubscriber* v2 ) const
    {
        if ( v1 > v2) {
            return -1;
        }
        return (v2 > v1) ? 1 : 0;
    }
};

typedef cds::container::IterableList< cds::gc::HP, ChatSubscriber*,
        typename cds::container::iterable_list::make_traits<
        cds::container::opt::compare< StupidPointerComparator >
        >::type
        >     option_based_list;

class ChatRoom
{
    option_based_list _list;
public:
    ChatRoom();
    ~ChatRoom();
    void addMessage(ChatMessage::ChatMessage & msg);
    void subscribe(ChatSubscriber* sub);
    void unsubscribe(ChatSubscriber* sub);
};

#endif // CHATROOM_H
