#ifndef MULTITHREADFBEXECUTOR_H
#define MULTITHREADFBEXECUTOR_H

#include <stdio.h>
#include <unistd.h> 
#include <string>
#include <thread>
#include <atomic>
#include <set>

#include "ThreadPool.h"
#include "FBExecutor.h"
#include "MultiSemaphore.h"

using namespace std;

extern FuncBlock* pFb;

extern atomic<bool> isStopSystem;

class MultiThreadFBExecutor : public FBExecutor {
    int nThread;
    int samplingTimersMillis;

public:    

    MultiThreadFBExecutor(int nThread) {
        this->nThread = nThread;
        this->samplingTimersMillis = 100;
        isStopSystem = false;
    }

    void timersThread() {
        while (true) {
            calculate();
            usleep(samplingTimersMillis * 1000);
        }
    }

    void run() {
        thread thread(&MultiThreadFBExecutor::timersThread, this);
        ctpl::thread_pool pool(nThread);
        set <string> markedToDelete;

        while (true) {
            eventSemaphore.waiting();

            markedToDelete.clear();

            for (auto fb : funcBlocks) {
                pFb = fb.second;

                if (pFb->isMarkedForDelete()) {
                    markedToDelete.insert(pFb->getName());
                    eventSemaphore.add(-pFb->mailbox->size);
                    continue;
                }

                pool.push([] (int id) {
                    pFb->run();
                });
            }

            for (auto fb : markedToDelete) {
                funcBlocks.erase(fb);
            }
        }
    }

    void stopSystem() {
        isStopSystem.store(true);
        bool isStop;
        do {
            isStop = true;
            for (auto fb : funcBlocks) {
                FuncBlock* pFb = fb.second;
                if (pFb->executeTaskLock.load()) {
                    isStop = false;
                }
            }
        } while (!isStop);
    }

    void releaseSystem() {
        isStopSystem.store(false);
    }
};
#endif /* MULTITHREADFBEXECUTOR_H */

