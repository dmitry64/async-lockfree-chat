#include "chatroom.h"

ChatRoom::ChatRoom()
{
    pthread_mutex_init(&_mutex, NULL);
}

void ChatRoom::addMessage(std::pair<std::__cxx11::string, std::__cxx11::string> &msg)
{
    pthread_mutex_lock(&_mutex);
    _messages.push_back(msg);
    pthread_mutex_unlock(&_mutex);
}
