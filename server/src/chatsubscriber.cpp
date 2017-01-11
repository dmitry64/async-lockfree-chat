#include "chatsubscriber.h"

ChatSubscriber::ChatSubscriber()
{
    _isDead = false;
}

ChatSubscriber::~ChatSubscriber()
{
    _isDead = true;
}

bool ChatSubscriber::isDead() const
{
    return _isDead;
}
