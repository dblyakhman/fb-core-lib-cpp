#ifndef FUNCBLOCK_H
#define FUNCBLOCK_H

#include <semaphore.h>
#include <pthread.h>
#include <errno.h>
#include <system_error>
#include <cerrno> 
#include <stdio.h> 
#include <string.h> 
#include <unistd.h> 
#include <atomic>

#include "FBEvent.h"
#include "MultiSemaphore.h"
#include "FBExecutor.h"

using namespace std;

extern atomic<bool> isStopSystem;
// ...........................................................................................
// ... Class Mailbox .......................................................................
// ...........................................................................................
#define FB_MAILBOX_SIZE       100
// ...........................................................................................

typedef unsigned char BYTE;

class Mailbox {
    FBEvent** pBox;
    int pWr, pRd; // указатели на массив в Box
    pthread_mutex_t lock;
    sem_t freeSpace;
    sem_t usedSpace;
    int mailboxSize;

public:
    int size;

    Mailbox(int mailboxSize) {
        pWr = pRd = 0;
        this->mailboxSize = mailboxSize;
        pBox = new FBEvent*[ mailboxSize ];
        size = 0;

        pthread_mutex_init(&lock, NULL);
        sem_init(&freeSpace, 0, mailboxSize);
        sem_init(&usedSpace, 0, 0);
    }
    //-------------------------------------------------------------------------------

    void clearMailbox(void) {
        pWr = pRd = 0;
        size = 0;
        pthread_mutex_init(&lock, NULL);
        sem_init(&freeSpace, 0, mailboxSize);
        sem_init(&usedSpace, 0, 0);
    }
    //-------------------------------------------------------------------------------

    bool offer(FBEvent* c) {
        if (sem_trywait(&freeSpace) == -1) return false;
        //-----------------------------------
        pthread_mutex_lock(&lock);
        pBox[ pWr ] = c;
        pWr++;
        pWr %= mailboxSize;
        size++;
        pthread_mutex_unlock(&lock);
        //-----------------------------------
        sem_post(&usedSpace);
        return true;
    }

    void put(FBEvent* c) {
        sem_wait(&freeSpace);
        //-----------------------------------
        pthread_mutex_lock(&lock);
        pBox[ pWr ] = c;
        pWr++;
        pWr %= mailboxSize;
        size++;
        pthread_mutex_unlock(&lock);
        //-----------------------------------
        sem_post(&usedSpace);
    }
    //-------------------------------------------------------------------------------

    void putFront(FBEvent* c) {
        sem_wait(&freeSpace);
        //-----------------------------------
        pthread_mutex_lock(&lock);
        if (pRd == 0) pBox[ (pRd = mailboxSize - 1) ] = c;
        else pBox[ --pRd ] = c;
        size++;
        pthread_mutex_unlock(&lock);
        //-----------------------------------
        sem_post(&usedSpace);
    }
    //-------------------------------------------------------------------------------

    FBEvent* take(void) {
        FBEvent* c;
        sem_wait(&usedSpace);
        //-----------------------------------
        pthread_mutex_lock(&lock);
        c = pBox[ pRd ];
        pRd++;
        pRd %= mailboxSize;
        size--;
        pthread_mutex_unlock(&lock);
        //-----------------------------------
        sem_post(&freeSpace);
        return c;
    }
    //-------------------------------------------------------------------------------

    bool isEmpty() {
        if (size == 0) return true;
        else return false;
    }
};
// ...........................................................................................
// ... Class FuncBlock .......................................................................
// ...........................................................................................
//void* ThreadTaskFunc( void* p );

class FuncBlock {
    unsigned mailboxSize;
    string name;

public:
    Mailbox* mailbox; //pthread_t ThreadTask;  
    FBExecutor* executor;
    atomic<bool> executeLock;
    atomic<bool> executeTaskLock;
    atomic<bool> markedForDelete;

    FuncBlock(FBExecutor* executor, string name, unsigned mailboxSize = 1000);

    virtual void task(FBEvent* event) {
    }

    string getName() {
        return name;
    }
    //-------------------------------------------------------------

    bool offer(FBEvent* c) {
        bool res = false;
        if (!isMarkedForDelete()) {
            res = mailbox->offer(c);
            if (res) {
                eventSemaphore.take();
            }
        }
        return res;
    }

    void put(FBEvent* c) {
        if (!isMarkedForDelete()) {
            mailbox->put(c);
            eventSemaphore.take();
        }
    }

    void putFront(FBEvent* c) {
        if (!isMarkedForDelete()) {
            mailbox->putFront(c);
            eventSemaphore.take();
        }
    }
    //-------------------------------------------------------------

    FBEvent* take(void) {
        return mailbox->take();
    }
    //-------------------------------------------------------------

    void clearMailbox(void) {
        mailbox->clearMailbox();
    }

    void run() {
        if (catchExecuteLock()) {
            while (!mailbox->isEmpty() && !isMarkedForDelete()) {
                while (isStopSystem.load()) {
                    usleep(10 * 1000);
                }

                FBEvent* event = mailbox->take();
                executeTaskLock = true;
                task(event);
                executeTaskLock = false;
                eventSemaphore.release();
            }
            executeLock = false;
        }
    }

    bool isEmpty() {
        return mailbox->isEmpty();
    }

    bool isMarkedForDelete() {
        return markedForDelete.load();
    }

    void markForDelete() {
        markedForDelete.store(true);
    }

    FBTimer* startTimer(FBTimerType type, FBEvent* callbackEvent, int expires) {
        FBTimer* timer = executor->startTimer(getName(), type, callbackEvent, expires);
        return timer;
    }

    void deleteTimer(FBTimer* timer) {
        executor->deleteTimer(timer);
    }

    bool catchExecuteLock() {
        bool b1 = false;
        bool b2 = true;
        return executeLock.compare_exchange_strong(b1, b2);
    }
    //-------------------------------------------------------------
    friend void operator>>(int, FuncBlock&);
    //-------------------------------------------------------------
};

#endif /* FUNCBLOCK_H */

