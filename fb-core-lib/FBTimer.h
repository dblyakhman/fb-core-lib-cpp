#ifndef FBTIMER_H
#define FBTIMER_H

#include <unistd.h>
#include <string.h> 
#include <pthread.h>
#include <unordered_map> 
#include <set> 
#include <atomic> 

#include "FBEvent.h"
#include "FBTimerType.h"

using namespace std;

void* threadTaskFunc(void* p);

class FBTimer {
    string name;

    FBTimerType type;

    int expires;

    FBEvent* callbackEvent;

    long startTime;

    atomic<bool> markedForDelete;

public:

    FBTimer(string name, FBTimerType type, FBEvent* callbackEvent, int expires) {
        this->name = name;
        this->type = type;
        this->expires = expires;
        this->callbackEvent = callbackEvent;
        this->markedForDelete = false;

        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        this->startTime = ts.tv_sec * 1000 + (ts.tv_nsec / 1000000);
    }

    string getName() {
        return name;
    }

    FBTimerType getType() {
        return type;
    }

    int getExpires() {
        return expires;
    }

    long getStartTime() {
        return startTime;
    }

    void setStartTime(long startTime) {
        this->startTime = startTime;
    }

    FBEvent* getCallbackEvent() {
        return callbackEvent;
    }

    bool isMarkedForDelete() {
        return markedForDelete.load();
    }

    void setMarkedForDelete(bool markedForDelete) {
        this->markedForDelete.store(markedForDelete);
    }
};

#endif /* FBTIMER_H */

