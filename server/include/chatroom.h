#ifndef CHATROOM_H
#define CHATROOM_H
#include <string>
#include <vector>
#include <deque>
//#include <mutex>
#include <boost/shared_ptr.hpp>
#include "chatsubscriber.h"
#include "ChatMessage.pb.h"
//#include <boost/thread/shared_mutex.hpp>
#include <cds/container/iterable_list_hp.h>

struct Foo_cmp {
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
        cds::container::opt::compare< Foo_cmp >     // item comparator option
        >::type
        >     option_based_list;

class ChatRoom
{
    //pthread_mutex_t _mutex;
    //boost::lockfree::stack< std::pair<unsigned int, ChatMessage::ChatMessage> > _messages;
    //std::vector< std::pair<unsigned int, ChatMessage::ChatMessage> > _messages;
    //cds::container::SplitListSet< cds::gc::DHP, ChatSubscriber*, > subs;
    //std::deque<ChatSubscriber* > _subscribers;
    option_based_list _list;
    //mutable boost::shared_mutex _connectionsMuntex;
public:
    ChatRoom();
    ~ChatRoom();
    void addMessage(ChatMessage::ChatMessage & msg);
    void subscribe(ChatSubscriber* sub);
    void unsubscribe(ChatSubscriber* sub);
};

#endif // CHATROOM_H
