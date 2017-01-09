#ifndef CHATROOM_H
#define CHATROOM_H
#include <string>
#include <vector>
#include <mutex>

class ChatRoom
{
    pthread_mutex_t _mutex;
    std::vector<std::pair<std::string, std::string> > _messages;
public:
    ChatRoom();
    void addMessage(std::pair<std::string, std::string> & msg);
};

#endif // CHATROOM_H
