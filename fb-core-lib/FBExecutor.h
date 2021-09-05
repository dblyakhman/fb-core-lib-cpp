#ifndef FBEXECUTOR_H
#define FBEXECUTOR_H

#include <string.h> 
#include <pthread.h>
#include <unordered_map> 

#include "FBEvent.h"
#include "FBTimer.h"
#include "FBTimerType.h"

using namespace std;

void* threadTaskFunc(void* p);

class FuncBlock;

class FBExecutor {
public:    
    unordered_map<string, FuncBlock*> funcBlocks;
    unordered_map<FBTimer*, FBTimer*> timers;

    pthread_t threadTask;

    FBExecutor() {
        pthread_create(&threadTask, NULL, threadTaskFunc, this);
    }

    void add(FuncBlock* fb);

    FuncBlock* get(string name);

    void markForDelete(string name);

    void offer(string name, FBEvent* event);

    void put(string name, FBEvent* event);

    FBTimer* startTimer(string name, FBTimerType type, FBEvent* callbackEvent, int expires);

    void deleteTimer(FBTimer* timer);

    void calculate();

    virtual void run(void) {
    }
};

#endif /* FBEXECUTOR_H */

