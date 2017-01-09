#ifndef CHATSUBSCRIBER_H
#define CHATSUBSCRIBER_H


class ChatSubscriber
{
public:
    ChatSubscriber();
    virtual unsigned int getLastReceivedMessageNumber() = 0;
    virtual void prepareToSync() = 0;
};

#endif // CHATSUBSCRIBER_H
